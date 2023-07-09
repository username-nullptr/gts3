#include "session/request_impl.h"
#include "gts/algorithm.h"
#include "gts/log.h"

namespace gts { namespace http
{

request::request() :
	m_impl(new request_impl())
{

}

request::request(request &&other) :
	m_impl(other.m_impl)
{
	assert(other.m_impl);
	other.m_impl = nullptr;
}

request::~request()
{
	if( m_impl )
		delete m_impl;
}

method request::method() const
{
	assert(m_impl);
	return m_impl->m_method;
}

std::string request::version() const
{
	assert(m_impl);
	return m_impl->m_version;
}

std::string request::path() const
{
	assert(m_impl);
	return m_impl->m_path;
}

std::string request::parameters_string() const
{
	assert(m_impl);
	return m_impl->m_parameters_string;
}

const parameters &request::parameters() const
{
	assert(m_impl);
	return m_impl->m_parameters;
}

const headers &request::headers() const
{
	assert(m_impl);
	return m_impl->m_headers;
}

std::string request::read_body(std::size_t size)
{
	assert(m_impl);
	if( size == 0 )
		size = m_impl->tcp_ip_buffer_size();

	auto content_length = headers().value_uint("content-length", 0);
	if( content_length < size )
		size = content_length;

	std::string result;
	if( size == 0 )
		return result;

	if( not m_impl->m_body.empty() )
	{
		if( size < m_impl->m_body.size() )
		{
			result = m_impl->m_body.substr(0, size);
			m_impl->m_body.erase(0, size);
			return result;
		}
		else if( size == m_impl->m_body.size() )
		{
			result = std::move(m_impl->m_body);
			return result;
		}
		result = std::move(m_impl->m_body);
		size -= result.size();
	}
	char *buf = new char[size] {0};
	auto res = m_impl->m_socket->read_some(buf, size);

	if( res > 0 )
		result += std::string(buf, res);

	delete[] buf;
	return result;
}

std::size_t request::read_body(void *buf, std::size_t size)
{
	assert(m_impl);
	auto content_length = headers().value_uint("content-length", 0);
	if( size == 0 )
	{
		log_warning("request::read_body: size is 0.");
		return 0;
	}
	if( content_length < size )
		size = content_length;
	if( size == 0 )
		return 0;

	std::size_t offset = 0;
	if( not m_impl->m_body.empty() )
	{
		if( size < m_impl->m_body.size() )
		{
			memcpy(buf, m_impl->m_body.c_str(), size);
			m_impl->m_body.erase(0, size);
			return size;
		}
		memcpy(buf, m_impl->m_body.c_str(), m_impl->m_body.size());
		if( size == m_impl->m_body.size() )
		{
			m_impl->m_body.clear();
			return size;
		}
		size -= m_impl->m_body.size();
		offset += m_impl->m_body.size();
		m_impl->m_body.clear();
	}
	auto res = m_impl->m_socket->read_some(static_cast<char*>(buf) + offset, size);
	return offset + (res > 0 ? res : 0);
}

bool request::keep_alive() const
{
	assert(m_impl);
	return m_impl->m_keep_alive;
}

bool request::support_gzip() const
{
	assert(m_impl);
	return m_impl->m_support_gzip;
}

request &request::operator=(request &&other)
{
	assert(other.m_impl);
	if( m_impl )
		delete m_impl;

	m_impl = other.m_impl;
	other.m_impl = nullptr;
	return *this;
}

bool request::is_valid() const
{
	return m_impl and not m_impl->m_version.empty() and not m_impl->m_path.empty();
}

const tcp_socket &request::socket() const
{
	assert(m_impl);
	return *m_impl->m_socket;
}

tcp_socket &request::socket()
{
	assert(m_impl);
	return *m_impl->m_socket;
}

}} //namespace gts::http
