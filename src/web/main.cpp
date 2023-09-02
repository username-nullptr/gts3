#include "session/session.h"
#include "gts/web/thread_pool.h"
#include "gts/web/config_key.h"
#include "gts/web_global.h"

#include "gts/registration.h"
#include "gts/application.h"
#include "gts/algorithm.h"
#include "gts/settings.h"
#include "gts/log.h"

#include <iostream>
#include <thread>

namespace gts { namespace web
{

static asio::thread_pool *g_pool = nullptr;

static std::size_t g_count = 0;

asio::thread_pool &thread_pool()
{
	assert(g_pool);
	return *g_pool;
}

std::size_t thread_pool_count()
{
	return g_count;
}

static std::string g_resource_path = _GTS_WEB_DEFAULT_RC_PATH;

std::string resource_root()
{
	return g_resource_path;
}

}} //namespace gts::web

GTS_PLUGIN_REGISTRATION
{
	using namespace gts;
	using namespace gts::web;

	gts::registration()
		.new_connection([](tcp_socket_ptr &socket){
			session::new_connection(std::move(socket));
		})
		.view_status_method([]{
			return session::view_status();
		})
		.init_method([]
		{
			gts_log_debug("gts web plugin init. (config file: {})", settings::global_instance().file_name());
			settings::ini_hash sample_gts
			{
				{ SINI_WEB_DEFAULT_RESOURCE  , "/index.html"             },
				{ SINI_WEB_CGI_ENV           , ""                        },
				{ SINI_WEB_CGI_ACCESS        , "/cgi-bin"                },
				{ SINI_WEB_CGI_PATH          , _GTS_WEB_DEFAULT_CGI_PATH },
				{ SINI_WEB_RC_PATH           , _GTS_WEB_DEFAULT_RC_PATH  },
				{ SINI_WEB_IDLE_TIME_TV      , 2048                      },
				{ SINI_WEB_MAX_IDLE_TIME     , 65536                     },
				{ SINI_WEB_MAX_SESSION_COUNT , 4096                      },
				{ SINI_WEB_THREAD_POOL_TC    , 255                       },
				{ SINI_WEB_ENABLE_WSS        , false                     },
				{ SINI_WEB_WSS_NAME          , ""                        },
				{ SINI_WEB_WSS_PORT          , ""                        }
			};
			settings::ini_file_check(SINI_GROUP_WEB, sample_gts);

			auto &_settings = settings::global_instance();
			g_resource_path = _settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_RC_PATH, g_resource_path);

			g_resource_path = app::absolute_path(g_resource_path);
			if( not str_ends_with(g_resource_path, "/") )
				g_resource_path += "/";

			g_count = _settings.read<int>(SINI_GROUP_WEB, SINI_WEB_THREAD_POOL_TC, 255);
			if( g_count < 1 )
			{
				gts_log_warning("Config: max thread count setting error.");
				g_count = 1;
			}
			gts_log_debug("Web: max thread count: {}", g_count);
			g_pool = new asio::thread_pool(g_count);
			session::init();
		})
		.exit_method([]
		{
			gts_log_debug("web plugin exit.");
			if( g_pool == nullptr )
				return ;

			g_pool->join();
			delete g_pool;
			g_pool = nullptr;
		});
}
