#include "application.h"
#include "gts/algorithm.h"
#include "gts/gts_config_key.h"
#include "app_info.h"
#include "gts_log.h"
#include "global.h"

#include "cmdline/app_core.h"
#include <rttr/variant.h>

#include <cppfilesystem>
#include <iostream>
#include <atomic>
#include <cassert>

#if defined(__unix__) && defined(GTS_DEBUG)
# include <execinfo.h>
# include <unistd.h>
# include <cstdio>
#endif //__unix__ && debug

#include <ctime>

namespace gts
{

static class GTS_DECL_HIDDEN applictaion_impl
{
	DISABLE_COPY(applictaion_impl)

public:
	applictaion_impl(int argc, const char *argv[]);
	~applictaion_impl();

private:
	void set_config_file(const cmdline::argument_hash &args_hash);

public:
	std::list<std::string> m_args;

public:
#ifdef __unix__
	asio::signal_set m_sigs;
#endif //__unix__

public:
	std::atomic_bool m_exit {false};
	std::atomic_int m_exit_code {0};
}
*g_impl = nullptr;

#if defined(__unix__) && defined(GTS_DEBUG)
static __sighandler_t pre_signal_hander = nullptr;
static void signal_hander(int signo);
#endif //__unix__

applictaion_impl::applictaion_impl(int argc, const char *argv[])
#ifdef __unix__
	: m_sigs(gts::io_context(), SIGINT, SIGTERM, SIGPIPE)
#endif //__unix__
{
	if( appinfo::set_current_directory(appinfo::dir_path()) == false )
		log_fatal("set_current_directory failed: {}. ({})", strerror(errno), errno);
	set_config_file(cmdline::startup(argc, argv));

	log::logger::context context;
	auto &settings = gts::settings::global_instance();

	context.dir      = settings.read<std::string>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_DIR);
	context.category = settings.read<std::string>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_CATEGORY);
	context.mask     = settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_LEVEL);
	context.async    = settings.read<bool>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_ASYNC);

	context.max_size_one_file = settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_MAXOF);
	context.max_size_one_day  = settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_MAXOD);
	context.max_size          = settings.read<int>(SINI_GROUP_GTSLOG, SINI_GTS_LOG_MAX);

	log::logger::set_context(context);
	log::logger::set_header_breaks_aline(true);

	for(int i=0; i<argc; i++)
		m_args.emplace_back(argv[i]);

#ifdef __unix__
# ifdef GTS_DEBUG
	pre_signal_hander = signal(SIGSEGV, signal_hander);
	signal(SIGQUIT, signal_hander);
	signal(SIGBUS,  signal_hander);
	signal(SIGTRAP, signal_hander);
	signal(SIGFPE,  signal_hander);
	signal(SIGABRT, signal_hander);
# endif //GTS_DEBUG

	m_sigs.async_wait([this](const asio::error_code&, int signo)
	{
		log_info("system signal: {}.", signo);

		if( signo == SIGINT or signo == SIGTERM )
		{
			m_exit = true;
			gts::io_context().stop();
		}

		else if( signo == SIGPIPE )
			log_info("pipe rupture. (ignore)");
	});
#endif //__unix__
}

applictaion_impl::~applictaion_impl()
{
	cmdline::app_unlock();
}

void applictaion_impl::set_config_file(const cmdline::argument_hash &args_hash)
{
	std::string file_name;
	auto it = args_hash.find(cmdline::sa_cfpath);

	if( it == args_hash.end() )
		file_name = appinfo::dir_path() + "config.ini";
	else
	{
		file_name = it->second;
		if( not starts_with(file_name, _PS) )
			file_name = appinfo::dir_path() + file_name;
	}

	log_debug("Using config file '{}' ...", file_name);

	if( not fs::exists(file_name) )
	{
		log_warning("Config file '{}' not exists. (auto create)", file_name);
		auto fp = std::fopen(file_name.c_str(), "w");

		if( fp == nullptr )
			log_error("Config file '{}' create failed: {}.", file_name, strerror(errno));
		else
			std::fclose(fp);
	}

	auto &_settings = settings::global_instance();
	_settings.set_delete_on_flush(false);
	_settings.set_file(file_name);

	settings::ini_hash sample_gts =
	{
		{ SINI_GTS_ADDRESS      , "ipv4"                    },
		{ SINI_GTS_PORT         , 8080                      },
		{ SINI_GTS_TCP_BUF_SIZE , 65536                     },
		{ SINI_GTS_STRATEGY     , _GTS_DEFULT_STRATEGY      },
		{ SINI_GTS_SSCFG        , "subserver.json"          }
	};
	settings::ini_file_check(SINI_GROUP_GTS, sample_gts);

	settings::ini_hash sample_gtslog =
	{
		{ SINI_GTS_LOG_DIR      , ""         },
		{ SINI_GTS_LOG_LEVEL    , 4          },
		{ SINI_GTS_LOG_CATEGORY , "gts"      },
		{ SINI_GTS_LOG_ASYNC    , true       },
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
		log_debug("class 'application' exit.");
		delete g_impl;
		g_instance = nullptr;
		g_impl = nullptr;
	}
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

std::list<std::string> applictaion::args() const
{
	return g_impl->m_args;
}

int applictaion::exec()
{
	if( g_impl->m_exit )
		return g_impl->m_exit_code;

	auto &io = gts::io_context();
	asio::io_context::work io_work(io); (void)(io_work);

	io.run();
	return g_impl->m_exit_code;
}

void applictaion::exit(int code)
{
	g_impl->m_exit_code = code;
	gts::io_context().stop();
}

/*----------------------------------------------------------------------------------------------------------*/

#if defined(__unix__) && defined(GTS_DEBUG)
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
#endif //__unix__ && debug

} //namespace gts
