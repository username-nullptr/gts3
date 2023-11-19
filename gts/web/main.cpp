
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

#include "service/connection.h"
#include "gts/web/thread_pool.h"
#include "gts/web/config_key.h"

#include "gts/registration.h"
#include "gts/application.h"
#include "gts/settings.h"

GTS_WEB_NAMESPACE_BEGIN

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

GTS_PLUGIN_INIT()
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

	http::response::set_resource_root(g_resource_path);
	connection::init();
}

GTS_PLUGIN_EXIT()
{
	gts_log_debug("web plugin exit.");
	if( g_pool == nullptr )
		return ;

	g_pool->join();
	delete g_pool;
	g_pool = nullptr;
}

GTS_PLUGIN_VIEW_STATUS(){
	return connection::view_status();
}

GTS_PLUGIN_NEW_CONNECTION(socket){
	connection::new_connection(std::move(socket));
}

GTS_WEB_NAMESPACE_END
