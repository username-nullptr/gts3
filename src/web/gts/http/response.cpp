#include "response.h"

namespace gts { namespace http
{

response::response(status s) :
	m_status(s)
{

}

response::response(const headers &h, status s) :
	response(s)
{
	m_headers = h;
}

response::response(const std::string &v, status s) :
	response(s)
{
	m_version = v;
}

response::response(const std::string &v, const headers &h, status s) :
	response(h,s)
{
	m_version = v;
}

response::~response()
{

}

void response::set_status(status s)
{
	m_status = s;
}

void response::set_header(const std::string &key, const std::string &value)
{
	auto res = m_headers.emplace(key, value);
	if( res.second == false and res.first != m_headers.end() )
		res.first->second = value;
}

std::string response::to_string(bool end) const
{
	auto result = fmt::format("HTTP/{} {} {}\r\n", m_version, static_cast<int>(m_status),
							  status_description(m_status));

	for(auto &header : m_headers)
		result += header.first + ": " + header.second + "\r\n";

	if( end )
	{
		if( m_headers.find("content-length") == m_headers.end() and
			m_headers.find("CONTENT-LENGTH") == m_headers.end() and
			m_headers.find("Content-Length") == m_headers.end() )
			result += "context-length : 0\r\n";
		result += "\r\n";
	}
	return result;
}

}} //namespace gts::http
