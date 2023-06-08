#include "service.h"

namespace gts { namespace web
{

void service::init(const basic_string_list &args)
{
	service_io_config::init();
	plugin_service_config::init(args);
	cgi_service_config::init();
}

void service::exit()
{
	plugin_service_config::exit();
}

}} //gts::web
