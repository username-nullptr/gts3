#ifndef SERVICE_IO_H
#define SERVICE_IO_H

#include "socket.h"
#include "http/response.h"
#include "http/request.h"

namespace gts { namespace web
{

template <class asio_socket>
class GTS_DECL_HIDDEN service_io
{
public:
	typedef gts::socket<asio_socket>  tcp_socket;

public:
	service_io(tcp_socket &socket, http::request &request);
	void return_to_null(http::status status = http::hs_ok);

public:
	tcp_socket &socket;
	http::request &request;
	http::response response;
	std::string url_name;
};

class GTS_DECL_HIDDEN service_io_config
{
public:
	static std::string resource_path;
	static void init();
};

template <class asio_socket>
service_io<asio_socket>::service_io(tcp_socket &socket, http::request &request) :
	socket(socket), request(request), response(request.version())
{
	response.set_header("server", "gts3");

	if( request.version() == "1.1" )
	{
		if( not request.keep_alive() )
			response.set_header("connection", "close");
	}
}

template <class asio_socket>
void service_io<asio_socket>::return_to_null(http::status status)
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


#endif //SERVICE_IO_H
