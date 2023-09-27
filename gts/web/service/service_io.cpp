#include "service_io.h"
#include "gts/http/request_impl.h"

namespace gts { namespace web
{

service_io::service_io(http::service_context &context) :
	context(context), socket(context.socket())
{	
	auto &request = this->request();
	auto &response = this->response();

	response.set_header("server", "gts3");
	auto it = request.headers().find("origin");

	if( it != request.headers().end() )
		response.set_header("access-control-allow-origin", it->second);

	if( request.version() == "1.1" )
	{
		if( not request.keep_alive() )
			response.set_header("connection", "close");
	}
}

void service_io::return_to_null(http::status status)
{
	auto body = fmt::format("{} ({})", http::status_description(status), status);

	response().set_header("content-length", body.size())
			.set_status(status)
			.write(body);

	if( not request().keep_alive() )
		response().close(true);
}

http::request &service_io::request()
{
	return context.request();
}

http::response &service_io::response()
{
	return context.response();
}

}} //namespace gts::web