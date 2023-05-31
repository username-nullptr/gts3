#include "session/session.h"
#include "gts/settings.h"
#include "gts/log.h"

#include "gts/gts_config_key.h"
#include "gts/web_config_key.h"
#include "gts/algorithm.h"

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
		{ SINI_WEB_CGI_ENV           , ""                              },
		{ SINI_WEB_CGI_PATH          , _GTS_WEB_DEFAULT_CGI_PATH       },
		{ SINI_WEB_PLUGINS_CONFIG    , _GTS_WEB_DEFAULT_PLUGINS_CONFIG },
		{ SINI_WEB_RC_PATH           , _GTS_WEB_DEFAULT_RC_PATH        },
		{ SINI_WEB_IDLE_TIME_TV      , 2048                            },
		{ SINI_WEB_MAX_IDLE_TIME     , 65536                           },
		{ SINI_WEB_MAX_SESSION_COUNT , 4096                            },
		{ SINI_WEB_THREAD_POOL_TC    , 255                             },
		{ SINI_WEB_ENABLE_WSS        , false                           },
		{ SINI_WEB_WSS_NAME          , ""                              },
		{ SINI_WEB_WSS_PORT          , ""                              },
		{ SINI_WEB_WSS_STRATEGY      , ""                              },
	};
	settings::ini_file_check(SINI_GROUP_WEB, sample_gts);
	session_config::init();

	g_run_thread = std::thread([]
	{
		auto &io = io_context();
		asio::io_context::work io_work(io); (void)(io_work);
		io.run();
	});
}

GTS_DECL_EXPORT void exit()
{
	log_debug("web plugin exit.");
	io_context().stop();

	if( g_run_thread.joinable() )
		g_run_thread.join();

	session_config::exit();
}

GTS_DECL_EXPORT void new_connection(tcp::socket::native_handle_type handle, void* ssl, bool ipv6)
{
	static auto &io = io_context();
	tcp::socket tcp_socket(io, ipv6? tcp::v6() : tcp::v4(), handle);

#ifdef GTS_ENABLE_SSL
	if( ssl )
		session<ssl_stream>::new_connection(std::make_shared<socket<ssl_stream>>(std::move(tcp_socket),
											reinterpret_cast<ssl_stream::native_handle_type>(ssl)));
	else
#endif //ssl
		session<tcp::socket>::new_connection(std::make_shared<socket<tcp::socket>>(std::move(tcp_socket)));
}

GTS_DECL_EXPORT std::string view_status()
{
	return session_config::view_status();
}

}}} //namespace gts::web::plugin_main

using namespace gts::web;

#define GTS_PLUGIN  "gts.plugin."

RTTR_PLUGIN_REGISTRATION
{
	rttr::registration::
			method(GTS_PLUGIN "init", plugin_main::init)
			.method(GTS_PLUGIN "exit", plugin_main::exit)
			.method(GTS_PLUGIN "new_connection", plugin_main::new_connection)
			.method(GTS_PLUGIN "view_status", plugin_main::view_status);
}
