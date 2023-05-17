#include "session.h"
#include "gts/algorithm.h"
#include "gts/gts_config_key.h"
#include "gts/web_config_key.h"
#include "settings.h"
#include "gts_log.h"

#include <rttr/registration>
#include <iostream>
#include <thread>

#include <sys/socket.h>
#include <sys/types.h>

namespace gts { namespace web { namespace plugin_main
{

static std::thread g_run_thread;

GTS_DECL_EXPORT void init(const std::string &config_file)
{
	log_debug("gts web plugin init. (config file: {})", config_file);

	settings::ini_hash sample_gts =
	{
		{ SINI_WEB_CGI_ENV          , ""                              },
		{ SINI_WEB_CGI_PATH         , _GTS_WEB_DEFAULT_CGI_PATH       },
		{ SINI_WEB_PLUGINS_CONFIG   , _GTS_WEB_DEFAULT_PLUGINS_CONFIG },
		{ SINI_WEB_RC_PATH          , _GTS_WEB_DEFAULT_RC_PATH        },
		{ SINI_WEB_NET_TIMEOUT      , 10000                           },
		{ SINI_WEB_MAX_TASK_COUNT   , 255                             },
		{ SINI_WEB_BLOCK_QUEUE_SIZE , 1024                            },
		{ SINI_WEB_ENABLE_SSL       , false                           },
		{ SINI_WEB_CRT_FILE         , ""                              },
		{ SINI_WEB_KEY_FILE         , ""                              },
		{ SINI_WEB_ENABLE_WSS       , false                           },
		{ SINI_WEB_WSS_NAME         , ""                              },
		{ SINI_WEB_WSS_PORT         , ""                              },
		{ SINI_WEB_WSS_STRATEGY     , ""                              },
	};
	settings::ini_file_check(SINI_GROUP_WEB, sample_gts);
	session::init();

	g_run_thread = std::thread([]
	{
		auto &io = io_context();
		asio::io_context::work io_work(io); (void)(io_work);
		io.run();
	});
}

GTS_DECL_EXPORT void exit()
{
	io_context().stop();
	if( g_run_thread.joinable() )
		g_run_thread.join();
	session::exit();
}

GTS_DECL_EXPORT void new_connect(tcp::socket::native_handle_type handle, int protocol)
{
	auto &io = io_context();
	if( protocol == 4 )
		new session(std::make_shared<tcp::socket>(io, tcp::v4(), handle));
	else
		new session(std::make_shared<tcp::socket>(io, tcp::v6(), handle));
}

GTS_DECL_EXPORT std::string view_status()
{
	return fmt::format("session count: {}\n", session::count());
}

}}} //namespace gts::web::plugin_main

using namespace gts::web;

#define GTS_PLUGIN  "gts.plugin."

RTTR_PLUGIN_REGISTRATION
{
	rttr::registration::
			method(GTS_PLUGIN "init", plugin_main::init)
			.method(GTS_PLUGIN "exit", plugin_main::exit)
			.method(GTS_PLUGIN "new_connect", plugin_main::new_connect)
			.method(GTS_PLUGIN "view_status", plugin_main::view_status);
}
