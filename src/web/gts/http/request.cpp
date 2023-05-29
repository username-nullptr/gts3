#include "request.h"
#include "gts/algorithm.h"

namespace gts { namespace http
{

request::request()
{

}

request::~request()
{

}

std::string request::method() const
{
	return m_method;
}

std::string request::version() const
{
	return m_version;
}

std::string request::path() const
{
	return m_path;
}

std::string request::parameters_string() const
{
	return m_parameters_string;
}

const http::parameters &request::parameters() const
{
	return m_parameters;
}

const http::headers &request::headers() const
{
	return m_headers;
}

const std::string &request::body() const
{
	return m_body;
}

bool request::keep_alive() const
{
	return m_keep_alive;
}

bool request::support_gzip() const
{
	return m_gzip;
}

bool request::is_valid() const
{
	return not m_method.empty();
}

void request::finish()
{
	auto it = m_headers.find("connection");
	if( it == m_headers.end() )
		m_keep_alive = m_version != "1.0";
	else
		m_keep_alive = to_lower(it->second) != "close";

	it = m_headers.find("accept-encoding");
	if( it == m_headers.end() )
		m_gzip = false;
	else
	{
		for(auto &str : string_split(it->second, ","))
		{
			if( to_lower(trimmed(str)) == "gzip" )
			{
				m_gzip = true;
				break;
			}
		}
	}
}

}} //namespace gts::http
