#include "task.h"

namespace gts { namespace web
{

asio::thread_pool *scheduler::pool = nullptr;

std::string scheduler::cgi_path = _GTS_WEB_DEFAULT_CGI_PATH;

void scheduler::init()
{
	auto &_settings = settings::global_instance();
	int max_thread_count = _settings.read<int>(SINI_GROUP_WEB, SINI_WEB_THREAD_POOL_TC, 100);

	if( max_thread_count < 1 )
	{
		log_warning("Config: max thread count setting error.");
		max_thread_count = 1;
	}
	log_debug("Web: max thread count: {}", max_thread_count);

	pool = new asio::thread_pool(max_thread_count);
	cgi_path = _settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_PATH, cgi_path);

	if( not starts_with(cgi_path, "/") )
		cgi_path = appinfo::dir_path() + cgi_path;

	if( not ends_with(cgi_path, "/") )
		cgi_path += "/";

	if( not fs::exists(cgi_path) )
		fs::create_directories(cgi_path);

	service::init();
}

void scheduler::exit()
{
	service::exit();
	if( pool == nullptr )
		return ;

	pool->join();
	delete pool;
	pool = nullptr;
}

}} //namespace gts::web
