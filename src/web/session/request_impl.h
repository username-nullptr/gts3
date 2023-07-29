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
	void finish();

public:
	template <typename Sesn>
	std::shared_ptr<Sesn> create_session()
	{
		auto ptr = http::make_session<Sesn>();
		m_cookies["session_id"] = ptr->id();
		if( m_response )
			m_response->set_cookie("session_id", ptr->id());
		return ptr;
	}

public:
	response *m_response = nullptr;

	tcp_socket_ptr m_socket;
	std::size_t m_rclenght = 0;

	http::method m_method;
	std::string m_version;
	std::string m_path;

	http::parameters m_parameters;
	http::headers m_headers;
	http::basic_cookies m_cookies;

	std::string m_parameters_string;
	std::string m_body;

	bool m_keep_alive = true;
	bool m_support_gzip = false;
};

}} //namespace gts::http


#endif //GTS_HTTP_REQUEST_IMPL_H
