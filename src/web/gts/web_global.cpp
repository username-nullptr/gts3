#include "web_global.h"
#include "http/global.h"
#include "web_config_key.h"

#include "gts/web_plugin_interface.h"
#include "gts/settings.h"
#include "gts/log.h"

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

void thread_pool_post(std::function<void()> func)
{
	asio::post(*g_pool, std::move(func));
}

void global_init()
{
	auto &_settings = settings::global_instance();
	g_count = _settings.read<int>(SINI_GROUP_WEB, SINI_WEB_THREAD_POOL_TC, 255);

	if( g_count < 1 )
	{
		log_warning("Config: max thread count setting error.");
		g_count = 1;
	}

	log_debug("Web: max thread count: {}", g_count);
	g_pool = new asio::thread_pool(g_count);
}

void global_exit()
{
	if( g_pool == nullptr )
		return ;

	g_pool->join();
	delete g_pool;
	g_pool = nullptr;
}

}} //namespace gts::web
