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

namespace gts { namespace web
{

extern GTS_DECL_HIDDEN void global_init();
extern GTS_DECL_HIDDEN void global_exit();

namespace plugin_main
{

GTS_DECL_EXPORT void init(const std::string &config_file)
{
	log_debug("gts web plugin init. (config file: {})", config_file);

	settings::ini_hash sample_gts =
	{
		{ SINI_WEB_DEFAULT_RESOURCE  , "/index.html"                   },
		{ SINI_WEB_CGI_ENV           , ""                              },
		{ SINI_WEB_CGI_ACCESS        , "/cgi-bin"                      },
		{ SINI_WEB_CGI_PATH          , _GTS_WEB_DEFAULT_CGI_PATH       },
		{ SINI_WEB_PLUGINS_CONFIG    , _GTS_WEB_DEFAULT_PLUGINS_CONFIG },
		{ SINI_WEB_PLUGINS_ACCESS    , "/plugin-lib"                   },
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
	global_init();
}

GTS_DECL_EXPORT void exit()
{
	log_debug("web plugin exit.");
	session_config::exit();
	global_exit();
}

template <typename asio_socket>
GTS_DECL_EXPORT void new_connection(gts::socket<asio_socket> &socket)
{
	session<asio_socket>::new_connection(std::make_shared<gts::socket<asio_socket>>(std::move(socket)));
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
			.method(GTS_PLUGIN "new_connection", plugin_main::new_connection<tcp::socket>)
#ifdef GTS_ENABLE_SSL
			.method(GTS_PLUGIN "new_connection", plugin_main::new_connection<gts::ssl_stream>)
#endif //ssl
			.method(GTS_PLUGIN "view_status", plugin_main::view_status);
}
