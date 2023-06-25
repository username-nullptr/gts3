#include "response.h"
#include <gts/log.h>
#include <cassert>

namespace gts { namespace http
{

class GTS_DECL_HIDDEN response_impl
{
public:
	tcp_socket_ptr m_socket;

	std::string m_version = "1.1";
	status m_status = hs_ok;

	headers m_headers {
		{ "content-type", "text/plain; charset=utf-8" }
	};
	bool m_headers_writed = false;
};

/*---------------------------------------------------------------------------------------------------------------*/

response::response(tcp_socket_ptr socket, http::status status) :
	m_impl(new response_impl())
{
	assert(socket);
	m_impl->m_socket = std::move(socket);
	m_impl->m_status = status;
}

response::response(tcp_socket_ptr socket, const http::headers &headers, http::status status) :
	response(std::move(socket), status)
{
	m_impl->m_headers = headers;
}

response::response(tcp_socket_ptr socket, const std::string &v, http::status status) :
	response(std::move(socket), status)
{
	m_impl->m_version = v;
}

response::response(tcp_socket_ptr socket, const std::string &v, const http::headers &headers, http::status status) :
	response(std::move(socket), headers, status)
{
	m_impl->m_version = v;
}

response::response(response &&other)
{
	assert(other.m_impl);
	m_impl = other.m_impl;

	delete other.m_impl;
	other.m_impl = nullptr;
}

response::~response()
{
	if( m_impl )
		delete m_impl;
}

void response::set_status(http::status status)
{
	m_impl->m_status = status;
}

void response::set_header(const std::string &key, const std::string &value)
{
	auto res = m_impl->m_headers.emplace(key, value);
	if( res.second == false and res.first != m_impl->m_headers.end() )
		res.first->second = value;
}

const std::string &response::version() const
{
	return m_impl->m_version;
}

const headers &response::headers() const
{
	return m_impl->m_headers;
}

status response::status() const
{
	return m_impl->m_status;
}

void response::write(const std::string &body, bool close)
{
	if( m_impl->m_headers_writed )
	{
		log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return ;
	}
	m_impl->m_headers_writed = true;

	auto result = fmt::format("HTTP/{} {} {}\r\n", m_impl->m_version, static_cast<int>(m_impl->m_status),
							  status_description(m_impl->m_status));

	for(auto &header : m_impl->m_headers)
		result += header.first + ": " + header.second + "\r\n";

	if( body.empty() )
	{
		m_impl->m_socket->write_some(result + "\r\n");
		if( close )
			m_impl->m_socket->close();
	}
	else
	{
		if( m_impl->m_headers.find("content-length") == m_impl->m_headers.end() and
			m_impl->m_headers.find("CONTENT-LENGTH") == m_impl->m_headers.end() and
			m_impl->m_headers.find("Content-Length") == m_impl->m_headers.end() )
			result += fmt::format("context-length: {}\r\n", body.size());

		m_impl->m_socket->write_some(result + "\r\n" + body);
		m_impl->m_socket->close();
	}
}

void response::write_body(const std::string &body, bool close)
{
	m_impl->m_socket->write_some(body);
}

void response::close(bool force)
{
	m_impl->m_socket->close(force);
}

const tcp_socket &response::socket() const
{
	return *m_impl->m_socket;
}

tcp_socket &response::socket()
{
	return *m_impl->m_socket;
}

response &response::operator=(response &&other)
{
	assert(other.m_impl);
	if( m_impl )
		delete m_impl;
	m_impl = other.m_impl;

	delete other.m_impl;
	other.m_impl = nullptr;
	return *this;
}

}} //namespace gts::http
