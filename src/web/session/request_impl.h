#ifndef GTS_HTTP_REQUEST_IMPL_H
#define GTS_HTTP_REQUEST_IMPL_H

#include "gts/http/request.h"
#include "gts/http/response.h"
#include "gts/http/session.h"
#include "gts/tcp_socket.h"

namespace gts { namespace http
{

class GTS_DECL_HIDDEN request_impl
{
public:
	std::size_t tcp_ip_buffer_size() const;
	void finish(std::string &body);

public:
	std::size_t read_body_length_mode(std::error_code &error, void *buf, std::size_t size);
	std::size_t read_body_chunked_mode(std::error_code &error, void *buf, std::size_t size);

public:
	bool m_keep_alive = true;
	bool m_support_gzip = false;

	enum class rb_status
	{
		length,
		wait_size,
		wait_content,
		wait_headers,
		chunked_end,
		finished
	}
	m_rb_status = rb_status::length;
	rttr::variant m_rb_context;

public:
	response *m_response = nullptr;
	tcp_socket_ptr m_socket;

	http::method m_method;
	std::string m_version;
	std::string m_path;

	http::parameters m_parameters;
	http::headers m_headers;
	http::basic_cookies m_cookies;

	std::string m_parameters_string;
	std::string m_body;
};

}} //namespace gts::http


#endif //GTS_HTTP_REQUEST_IMPL_H
