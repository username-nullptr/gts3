
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifdef __unix__

#include "app_core_p.h"
#include "gts/private/app_info.h"

#include "gts/algorithm.h"
#include "gts/log.h"

#include <cppfilesystem>
#include <iostream>
#include <fstream>
#include <thread>

#include <unistd.h>
#include <signal.h>
#include <sys/file.h>

namespace gts { namespace cmdline
{

static int become_child_process();

static int g_lock_file_fd = -1;

args_parser::arguments startup(int argc, const char *argv[])
{
	static bool isInit = false;
	if( isInit )
		gts_log_fatal("cmdline_handle repeat call.");
	isInit = true;

	args_parser::arguments args_hash;
	cmdline_handle(argc, argv, args_hash);

	int pipefd = -1;
	if( args_hash & GC_SA_DAEMON )
		pipefd = become_child_process();

	auto lock_file = app::lock_file_name();
	g_lock_file_fd = open(lock_file.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
	if( g_lock_file_fd < 0 )
		gts_log_fatal("Lock file '{}' open failed: {}.", lock_file, strerror(errno));

	else if( flock(g_lock_file_fd, LOCK_EX | LOCK_NB) < 0 )
	{
		std::cout << "The server is running." << std::endl;
		exit(0);
	}

	auto str = fmt::format("{}", getpid());
	if( write(g_lock_file_fd, str.c_str(), str.size()) < 0 )
		gts_log_fatal("Lock file '{}' write failed: {}.", lock_file, strerror(errno));

	if( pipefd > 0 )
	{
		if( setsid() < 0 )
			gts_log_fatal("setsid: {}.", strerror(errno));
		gts_log_info("Daemon process running.");

		if( write(pipefd, "ok", 2) < 0 )
			gts_log_error("Child process: pipe write failed: {}.", strerror(errno));
		close(pipefd);
	}
	return args_hash;
}

void app_unlock()
{
	close(g_lock_file_fd);
	fs::remove(app::lock_file_name());
}

#define __END(noreturn, code) \
({ \
	if( noreturn ) \
		::exit(code); \
	else \
		return ; \
})

static void trimmed(std::string &str);

void stop_app(bool noreturn)
{
	if( not fs::exists(app::lock_file_name()) )
	{
		std::cout << "The server is not running." << std::endl;
		__END(noreturn, 0);
	}

	std::fstream file(app::lock_file_name(), std::ios_base::in);
	if( not file.is_open() )
		gts_log_fatal("The file '{}' open failed: {}.", app::lock_file_name(), strerror(errno));

	char buf[1024] = "";
	file.read(buf, sizeof(buf));
	file.close();

	if( buf[0] == '\0' )
	{
		std::cout << "The server is not running." << std::endl;
		__END(noreturn, 0);
	}

	std::string str(buf);
	trimmed(str);

	pid_t pid = stoui32(str);
	if( pid < 1 )
	{
		fs::remove(app::lock_file_name());
		gts_log_fatal("PID error, server may have stopped. (auto rm pid-file)");
	}

	std::cout << "server pid is " << pid << "." << std::endl;

	for(int i=0; i<3; i++)
	{
		std::cout << "Attempting to stop the server ..." << std::endl;

		if( kill(pid, SIGTERM) < 0 )
		{
			std::cerr << "Unable to kill server process: " << strerror(errno) << std::endl;
			std::cout << "If the server has stopped, you can remove '" << app::lock_file_name() << "'." << std::endl;
			__END(true, -1);
		}

		for(int i=0; i<100; i++)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			if( kill(pid, 0) < 0 )
			{
				std::cout << "Server is stop." << std::endl;
				__END(noreturn, 0);
			}
		}
	}

	std::cerr << "Stop timeout." << std::endl;
	exit(1);
}

/*------------------------------------------------------------------------------------------------------------------*/

#define P_ERROR(str) \
	gts_log_fatal(str ": %s.", strerror(errno))

static void signal_handler(int signo);

static int become_child_process()
{
	int pipefd[2] = {-1, -1};
	if( pipe(pipefd) < 0 )
		P_ERROR("pipe");

	pid_t pid = fork();
	if( pid < 0 )
		P_ERROR("fork");

	else if( pid > 0 )
	{
		if( signal(SIGCHLD, signal_handler) == SIG_ERR )
			P_ERROR("Parent process: signal");

		char buf[64] = {};
		int res = read(pipefd[0], buf, 64);

		gts_log_info("\nParent process exit. child PID = {}.", pid);

		if( res < 0 )
			P_ERROR("Parent process: pipe read");

		else if( res == 0 )
			gts_log_fatal("Child process exit, daemon not start.");

		else if( strcmp(buf, "ok") )
		{
			gts_log_warning() << "Code error:" << __FILE__ << __LINE__;
			exit(1);
		}
		else
			exit(0);
	}
	return pipefd[1];
}

static void signal_handler(int signo)
{
	if( signo == SIGCHLD )
		gts_log_fatal("Child process exit, daemon not start.");
}

static void trimmed(std::string &str)
{
	auto it = str.begin();
	while( it != str.end() )
	{
		if( *it == ' ' or *it == '\t' or *it == '\n' )
			it = str.erase(it);
		else
			++it;
	}
}

}} //namespace gts::cmdline

#endif //__unix__
