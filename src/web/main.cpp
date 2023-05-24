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

#ifdef GTS_ENABLE_SSL
static ssl::context *g_ssl_context = nullptr;
#endif //ssl

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

#ifdef GTS_ENABLE_SSL
	if( settings::global_instance().read<bool>(SINI_GROUP_GTS, SINI_GTS_ENABLE_SSL) )
	{
		log_info("gts: enable ssl.");
		g_ssl_context = new ssl::context(ssl::context::sslv23);

		g_ssl_context->set_options(ssl::context::default_workarounds |
								   ssl::context::single_dh_use |
								   ssl::context::no_sslv2);
		asio::error_code error;

		g_ssl_context->set_password_callback([](std::size_t, ssl::context::password_purpose) -> std::string {
			return "seri";
		}, error);
		if( error )
			log_fatal("asio: ssl password failed: {}. ({})\n", error.message(), error.value());

		g_ssl_context->use_certificate_chain_file("/root/.ssl/server.crt", error);
		if( error )
			log_fatal("asio: ssl certificate file load failed: {}. ({})\n", error.message(), error.value());

		g_ssl_context->use_private_key_file("/root/.ssl/server.key", ssl::context::pem, error);
		if( error )
			log_fatal("asio: ssl private file file load failed: {}. ({})\n", error.message(), error.value());
	}
#endif //ssl

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

	if( g_ssl_context )
		delete g_ssl_context;

	session_config::exit();
}

GTS_DECL_EXPORT void new_connection(tcp::socket::native_handle_type handle, bool ssl, int protocol)
{
#ifdef GTS_ENABLE_SSL
	static auto &io = io_context();
	if( ssl and g_ssl_context )
		session<ssl_stream>::new_connection
				(std::make_shared<socket<ssl_stream>>(tcp::socket(io, protocol==4? tcp::v4():tcp::v6(), handle), *g_ssl_context));
	else
#endif //ssl
		session<tcp::socket>::new_connection
				(std::make_shared<socket<tcp::socket>>(io, protocol==4? tcp::v4():tcp::v6(), handle));
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
