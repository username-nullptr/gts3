#include "service.h"
#include "log.h"

namespace gts { namespace web
{

service::service(tcp::socket &socket, http::request &request) :
	socket(socket), request(request), response(request.version())
{
	response.set_header("server", "gts3");

	if( request.version() == "1.1" )
	{
		if( not request.keep_alive() )
			response.set_header("connection", "close");
	}
}

extern GTS_DECL_HIDDEN void plugin_service_init();
extern GTS_DECL_HIDDEN void plugin_service_exit();
extern GTS_DECL_HIDDEN void cgi_service_init();

void service::init()
{
	plugin_service_init();
	cgi_service_init();
}

void service::exit()
{
	plugin_service_exit();
}

void service::return_to_null(http::status status)
{
	response.set_status(status);
	response.set_header("content-length", "0");
	socket.write_some(asio::buffer(response.to_string()));

	if( not request.keep_alive() )
	{
		socket.shutdown(tcp::socket::shutdown_both);
		socket.close();
	}
}

}} //namespace gts::web
