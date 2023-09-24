#ifndef SERVICE_IO_H
#define SERVICE_IO_H

#include "gts/web/web_global.h"
#include "gts/http/service_context.h"
#include "gts/tcp_socket.h"

namespace gts { namespace web
{

class GTS_DECL_HIDDEN service_io
{
	GTS_DISABLE_COPY_MOVE(service_io)

public:
	explicit service_io(http::service_context &context);
	void return_to_null(http::status status = http::hs_ok);

public:
	http::request &request();
	http::response &response();

public:
	http::service_context &context;
	std::string url_name;
	tcp_socket &socket;
};

}} //namespace gts::web


#endif //SERVICE_IO_H
