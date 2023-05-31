#include "response.h"
#include <cassert>

namespace gts { namespace http
{

class GTS_DECL_HIDDEN response_private
{
public:
	std::string m_version = "1.1";
	status m_status = hs_ok;
	headers m_headers;
};

/*---------------------------------------------------------------------------------------------------------------*/

response::response(http::status status) :
	d_ptr(new response_private())
{
	d_ptr->m_status = status;
}

response::response(const http::headers &headers, http::status status) :
	response(status)
{
	d_ptr->m_headers = headers;
}

response::response(const std::string &v, http::status status) :
	response(status)
{
	d_ptr->m_version = v;
}

response::response(const std::string &v, const http::headers &headers, http::status status) :
	response(headers, status)
{
	d_ptr->m_version = v;
}

response::response(response &&other)
{
	assert(other.d_ptr);
	d_ptr = other.d_ptr;

	delete other.d_ptr;
	other.d_ptr = nullptr;
}

response::~response()
{
	if( d_ptr )
		delete d_ptr;
}

void response::set_status(http::status status)
{
	d_ptr->m_status = status;
}

void response::set_header(const std::string &key, const std::string &value)
{
	auto res = d_ptr->m_headers.emplace(key, value);
	if( res.second == false and res.first != d_ptr->m_headers.end() )
		res.first->second = value;
}

const std::string &response::version() const
{
	return d_ptr->m_version;
}

const headers &response::headers() const
{
	return d_ptr->m_headers;
}

status response::status() const
{
	return d_ptr->m_status;
}

std::string response::to_string(bool end) const
{
	auto result = fmt::format("HTTP/{} {} {}\r\n", d_ptr->m_version, static_cast<int>(d_ptr->m_status),
							  status_description(d_ptr->m_status));

	for(auto &header : d_ptr->m_headers)
		result += header.first + ": " + header.second + "\r\n";

	if( end )
	{
		if( d_ptr->m_headers.find("content-length") == d_ptr->m_headers.end() and
			d_ptr->m_headers.find("CONTENT-LENGTH") == d_ptr->m_headers.end() and
			d_ptr->m_headers.find("Content-Length") == d_ptr->m_headers.end() )
			result += "context-length : 0\r\n";
		result += "\r\n";
	}
	return result;
}

response &response::operator=(response &&other)
{
	assert(other.d_ptr);
	if( d_ptr )
		delete d_ptr;
	d_ptr = other.d_ptr;

	delete other.d_ptr;
	other.d_ptr = nullptr;
	return *this;
}

}} //namespace gts::http
