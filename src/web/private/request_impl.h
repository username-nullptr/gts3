#ifndef GTS_HTTP_REQUEST_IMPL_H
#define GTS_HTTP_REQUEST_IMPL_H

#include "gts/http/request.h"
#include "gts/tcp_socket.h"

namespace gts { namespace http
{

class GTS_DECL_HIDDEN request_impl
{
public:
	std::size_t tcp_ip_buffer_size() const;
	void finish();

public:
	tcp_socket_ptr m_socket;

	http::method m_method;
	std::string m_version;
	std::string m_path;

	http::parameters m_parameters;
	http::headers m_headers;

	std::string m_parameters_string;
	std::string m_body;

	bool m_keep_alive = true;
	bool m_support_gzip = false;
};

}} //namespace gts::http


#endif //GTS_HTTP_REQUEST_IMPL_H
