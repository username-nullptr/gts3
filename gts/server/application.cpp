
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

#include "application.h"
#include "server_tool.h"
#include "global.h"

#include "gts/gts_config_key.h"
#include "gts/algorithm.h"
#include "gts/log.h"

#include "cmdline/app_core.h"
#include "gts/private/app_info.h"

#include <cppfilesystem>
#include <iostream>
#include <atomic>
#include <cassert>

#ifdef __unix__
# include <execinfo.h>
# include <unistd.h>
# include <cstdio>
#endif //__unix__

GTS_NAMESPACE_BEGIN

static class GTS_DECL_HIDDEN applictaion_impl
{
	GTS_DISABLE_COPY_MOVE(applictaion_impl)

public:
	applictaion_impl(int argc, const char *argv[]);
	~applictaion_impl();

private:
	void set_config_file(const cmdline::args_parser::arguments &args_hash);

public:
	string_list m_args;

#ifdef __unix__
public:
	asio::signal_set m_sigs;
#endif //__unix__

public:
	std::atomic_bool m_exit {false};
	std::atomic_int m_exit_code {0};
}
*g_impl = nullptr;

#ifdef __unix__
static __sighandler_t pre_signal_hander = nullptr;
static void signal_hander(int signo);
#endif //__unix__

applictaion_impl::applictaion_impl(int argc, const char *argv[])
#ifdef __unix__
	: m_sigs(gts::io_context(), SIGINT, SIGTERM, SIGPIPE)
#endif //__unix__
{
	if( app::set_current_directory(app::dir_path()) == false )
		gts_log_fatal("set_current_directory failed: {}. ({})", strerror(errno), errno);

	cmdline::args_parser::arguments args_hash = cmdline::startup(argc, argv);
	set_config_file(args_hash);
	gts_log_info("Server instance name: '{}'.", app::instance_name());

	auto &_settings = gts::settings::global_instance();
	_settings.set_delete_on_flush(false);
	logger::context context;

	if( args_hash & GC_SA_INSNAME )
		context.category = args_hash.at(GC_SA_INSNAME);
	else
		context.category = "gts";

	context.dir   = _settings.read<std::string>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_DIR);
	context.mask  = _settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_LEVEL);
	context.async = _settings.read<bool>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_ASYNC);

	context.time_category     = _settings.read<bool>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_DIR_TMCY);
	context.max_size_one_file = _settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_MAXOF);
	context.max_size_one_day  = _settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_MAXOD);
	context.max_size          = _settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_MAX);

	logger::set_context(context);
	logger::set_header_breaks_aline(true);

	for(int i=0; i<argc; i++)
		m_args.emplace_back(argv[i]);

#ifdef __unix__
	pre_signal_hander = signal(SIGSEGV, signal_hander);
	signal(SIGQUIT, signal_hander);
	signal(SIGBUS,  signal_hander);
	signal(SIGTRAP, signal_hander);
	signal(SIGFPE,  signal_hander);
	signal(SIGABRT, signal_hander);

	m_sigs.async_wait([this](const asio::error_code&, int signo)
	{
		gts_log_info("system signal: {}.", signo);

		if( signo == SIGINT or signo == SIGTERM )
		{
			m_exit = true;
			gts::io_context().stop();
		}

		else if( signo == SIGPIPE )
			gts_log_info("pipe rupture. (ignore)");
	});
#endif //__unix__
}

applictaion_impl::~applictaion_impl()
{
	cmdline::app_unlock();
}

void applictaion_impl::set_config_file(const cmdline::args_parser::arguments &args_hash)
{
	std::string file_name;
	auto it = args_hash.find(GC_SA_CFPATH);

	if( it == args_hash.end() )
		file_name = app::dir_path() + "config.ini";
	else
		file_name = app::absolute_path(it->second);

	gts_log_debug("Using config file '{}' ...", file_name);

	if( not fs::exists(file_name) )
	{
		gts_log_warning("Config file '{}' not exists. (auto create)", file_name);

		if( fs::create_directories(file_path(file_name)) )
		{
			auto fp = std::fopen(file_name.c_str(), "w");

			if( fp == nullptr )
				gts_log_error("Config file '{}' create failed: {}.", file_name, strerror(errno));
			else
				std::fclose(fp);
		}
		else
			gts_log_error("Config file '{}' create failed: {}.", file_name, strerror(errno));
	}

	auto &_settings = settings::global_instance();
	_settings.set_delete_on_flush(false);
	_settings.set_file(file_name);

	settings::ini_hash sample_gts
	{
		{ SINI_GTS_PLUGINS_CONFIG , GTS_DEFAULT_PLUGINS_CONFIG },
		{ SINI_GTS_SITES_CONFIG   , GTS_DEFAULT_SITES_CONFIG   },
		{ SINI_GTS_TCP_BUF_SIZE   , 65536                      },
		{ SINI_GTS_SSL_CRT_FILE   , ""                         },
		{ SINI_GTS_SSL_KEY_FILE   , ""                         },
		{ SINI_GTS_SSL_KEY        , ""                         }
	};
	settings::ini_file_check(SINI_GROUP_GTS, sample_gts);

	settings::ini_hash sample_gtslog
	{
		{ SINI_GTS_LOG_DIR      , ""         },
		{ SINI_GTS_LOG_LEVEL    , 4          },
		{ SINI_GTS_LOG_ASYNC    , true       },
		{ SINI_GTS_LOG_DIR_TMCY , false      },
		{ SINI_GTS_LOG_MAXOF    , 10240      },
		{ SINI_GTS_LOG_MAXOD    , 10485760   },
		{ SINI_GTS_LOG_MAX      , 1073741824 },
	};
	settings::ini_file_check(SINI_GROUP_GTSLOG, sample_gtslog);
}

/*----------------------------------------------------------------------------------------------------------*/

static applictaion *g_instance = nullptr;
static std::atomic_int g_counter {0};

applictaion::applictaion(int argc, const char *argv[])
{
	if( g_counter++ == 0 )
	{
		g_impl = new applictaion_impl(argc, argv);
		g_instance = this;
	}
}

applictaion::~applictaion()
{
	if( --g_counter == 0 )
	{
		gts_log_debug("class 'application' exit.");
		delete g_impl;
		g_instance = nullptr;
		g_impl = nullptr;
	}
}

void applictaion::extension_init()
{
	plugin_call_handle::extension::init(settings::global_instance().file_name());
}

void applictaion::extension_exit()
{
	plugin_call_handle::extension::exit();
}

applictaion &applictaion::instance()
{
	assert(g_instance);
	return *g_instance;
}

asio::io_context &applictaion::io_context()
{
	return gts::io_context();
}

string_list applictaion::args() const
{
	return g_impl->m_args;
}

int applictaion::exec()
{
	if( g_impl->m_exit )
		return g_impl->m_exit_code;

	auto &io = gts::io_context();
	asio::io_context::work io_work(io); GTS_UNUSED(io_work);

	io.run();
	return g_impl->m_exit_code;
}

void applictaion::exit(int code)
{
	g_impl->m_exit_code = code;
	gts::io_context().stop();
}

/*----------------------------------------------------------------------------------------------------------*/

#ifdef __unix__
static void signal_hander(int signo)
{
	if( signo == SIGQUIT or signo == SIGSEGV or signo == SIGBUS or
		signo == SIGTRAP or signo == SIGFPE or signo == SIGABRT )
	{
		void *buf[256];
		int n = backtrace(buf, 256);

		auto fp = fopen("./core.stack", "w");
		fprintf(fp, "signal: %d\n", signo);
		fprintf(fp, "[%s]:[%d] n = %d\n", __func__, __LINE__, n);

		char **symbols = backtrace_symbols(buf, n);
		if( symbols == nullptr )
		{
			fprintf(fp, "backtrace_symbols");
			_exit(EXIT_FAILURE);
		}

		fprintf(fp, "[%s]:[%d]\n", __func__, __LINE__);
		for(int i=0; i<n; i++)
			fprintf(fp, "%d:%s\n", i, symbols[i]);

		fclose(fp);
		free(symbols);
		_exit(-signo);
	}

	else if( pre_signal_hander )
		pre_signal_hander(signo);
}
#endif //__unix__

GTS_NAMESPACE_END
