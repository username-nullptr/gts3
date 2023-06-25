#ifndef SERVICE_IO_H
#define SERVICE_IO_H

#include "gts/web_global.h"
#include "gts/http/response.h"
#include "gts/http/request.h"
#include "gts/tcp_socket.h"

namespace gts { namespace web
{

class GTS_DECL_HIDDEN service_io
{
public:
	service_io(tcp_socket_ptr socket, http::request &request);

public:
	static void init();
	static std::string resource_path();

public:
	void return_to_null(http::status status = http::hs_ok);

public:
	http::request &request;
	http::response response;
	std::string url_name;
};

}} //namespace gts::web


#endif //SERVICE_IO_H
