
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

#include "private/app_core_p.h"
#include "gts/private/app_info.h"
#include "gts/log.h"

#include "interaction.h"
#include "global.h"

#include <cppfilesystem>
#include <iostream>
#include <map>

namespace gts { namespace cmdline
{

static void server_is_running_check();

[[noreturn]] static void short_cmd_check(const args_parser::arguments &args_hash);

void cmdline_handle(int argc, const char *argv[], args_parser::arguments &args_hash)
{
	static bool isInit = false;
	if( isInit )
		gts_log_fatal("cmdline_handle repeat call.");
	isInit = true;

	string_list others;
	auto _args_hash = argument_check(argc, argv, others);

	if( _args_hash & GC_SA_INSNAME )
	{
		app::set_instance_name(_args_hash[GC_SA_INSNAME]);
		_args_hash.erase(GC_SA_INSNAME);
	}
	if( _args_hash & GC_SA_STATUS or
		_args_hash & GC_SA_VASUS or
		_args_hash & GC_SA_VRSUS or
		_args_hash & GC_SA_STSSA or
		_args_hash & GC_SA_SPSSA or
		_args_hash & GC_SA_STSS or
		_args_hash & GC_SA_SPSS )
		short_cmd_check(_args_hash);

	do {
		if( _args_hash & GC_SA_START )
		{
			if( not fs::exists(app::tmp_dir_path()) )
			{
				if( fs::create_directories(app::tmp_dir_path()) == false )
					gts_log_fatal("Run(tmp) diractory make failed!");
			}
			break;
		}
		else if( _args_hash & GC_SA_RESTART )
		{
			server_is_running_check();
			stop_app(false);
			break;
		}
		else if( _args_hash & GC_SA_STOP )
		{
			if( _args_hash.size() > 1 )
				gts_log_fatal("Too many arguments.");
			server_is_running_check();
			stop_app(true); //exit
		}

		std::cout << "If you want to start the server, add the argument 'start'." << std::endl;
		exit(-1);
	}
	while(0);
	args_hash = std::move(_args_hash);
}

/*----------------------------------------------------------------------------------------------------------------------*/

static void server_is_running_check()
{
	if( fs::exists(app::lock_file_name()) )
		return ;

	std::cout << "The server is not running." << std::endl;
	exit(1);
}

#ifdef _WINDOWS
# define _BUF_SIZE  8192
#elif defined(__unix__)
# define _BUF_SIZE  8192
#else //other os
# define _BUF_SIZE  8192
#endif //os

[[noreturn]] static void short_cmd_check(const args_parser::arguments &args_hash)
{
	std::string cmd_str;

	if( args_hash & GC_SA_STATUS )
		cmd_str = CCMD_VSS;
	else if( args_hash & GC_SA_VASUS )
		cmd_str = CCMD_VASS;
	else if( args_hash & GC_SA_VRSUS )
		cmd_str = CCMD_VRSS;
	else if( args_hash & GC_SA_STSSA )
		cmd_str = CCMD_STSSA;
	else if( args_hash & GC_SA_SPSSA )
		cmd_str = CCMD_SPSSA;
	else if( args_hash & GC_SA_STSS )
		cmd_str = CCMD_STSS + args_hash.at(GC_SA_STSS);
	else if( args_hash & GC_SA_SPSS )
		cmd_str = CCMD_SPSS + args_hash.at(GC_SA_SPSS);
	else
	{
		std::cerr << "Invalid arguments." << std::endl;
		exit(-1);
	}
	if( args_hash.size() > 1 )
	{
		std::cerr << "Too many arguments." << std::endl;
		exit(-1);
	}
	server_is_running_check();

	interaction _interaction(false);
	if( _interaction.open() == false )
		gts_log_fatal("cmd line interaction open failed.");

	int res = _interaction.write(cmd_str.c_str(), cmd_str.size(), 5000);
	if( res == 0 )
	{
		std::cerr << "Error: request operation timeout." << std::endl;
		exit(-1);
	}

	char buf[_BUF_SIZE] = "";
	res = _interaction.read(buf, _BUF_SIZE, 5000);

	if( res < 0 )
		exit(res);
	else if( res == 0 )
	{
		std::cerr << "Error: wait for reply operation timeout." << std::endl;
		exit(-1);
	}

	std::cout << buf << std::endl;
	exit(0);
}

}} //namespace gts::cmdline
