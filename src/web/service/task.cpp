#include "task.h"

namespace gts { namespace web
{

std::string task_config::cgi_path = _GTS_WEB_DEFAULT_CGI_PATH;

void task_config::init()
{
	cgi_path = settings::global_instance().read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_PATH, cgi_path);
	cgi_path = appinfo::absolute_path(cgi_path);

	if( not ends_with(cgi_path, "/") )
		cgi_path += "/";

	if( not fs::exists(cgi_path) )
		fs::create_directories(cgi_path);

	service::init();
}

void task_config::exit()
{
	service::exit();
}

std::string task_config::view_status()
{
	return plugin_service_config::view_status();
}

}} //namespace gts::web
