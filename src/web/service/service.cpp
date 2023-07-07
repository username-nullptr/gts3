#include "service.h"
#include "service_io.h"

namespace gts { namespace web
{

void service::call_plugin_service(service_io &sio)
{
	plugin_service(sio).call();
}

void service::call_cgi_service(service_io &sio)
{
	cgi_service(sio).call();
}

void service::init()
{
	plugin_service::init();
	cgi_service::init();
}

void service::exit()
{
	plugin_service::exit();
}

}} //gts::web
