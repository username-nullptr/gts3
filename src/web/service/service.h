#ifndef SERVICE_H
#define SERVICE_H

#include "global.h"

namespace gts { namespace web
{

namespace http{
class request;
class response;
} //namespace http

#define SEND_NULL_RESPONSE(_socket, _request, _response, _status) \
({ \
	(_response).set_status(_status); \
	(_response).set_header("content-length", "0"); \
	(_socket).write_some(asio::buffer(_response.to_string())); \
	if( not (_request).keep_alive() ) { \
		(_socket).shutdown(tcp::socket::shutdown_both); \
		(_socket).close(); \
	} \
})

GTS_DECL_HIDDEN void static_resource_service
(tcp::socket &socket, http::request &request, http::response &response, const std::string &url_name);

GTS_DECL_HIDDEN void plugin_service_init();
GTS_DECL_HIDDEN void plugin_service_exit();

GTS_DECL_HIDDEN void plugins_service
(tcp::socket &socket, http::request &request, http::response &response, const std::string &url_name);

GTS_DECL_HIDDEN void cgi_service_init();

GTS_DECL_HIDDEN void cgi_service
(tcp::socket &socket, http::request &request, http::response &response, const std::string &url_name);

}} //namespace gts::web


#endif //SERVICE_H
