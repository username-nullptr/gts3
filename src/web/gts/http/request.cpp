#include "session/request_impl.h"
#include "gts/application.h"
#include "gts/exception.h"
#include "gts/algorithm.h"
#include "gts/log.h"

#include <cppfilesystem>
#include <fstream>

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

const basic_cookies &request::cookies() const
{
	assert(m_impl);
	return m_impl->m_cookies;
}

const value &request::parameter(const std::string &key) const
{
	auto it = m_impl->m_parameters.find(key);
	if( it == m_impl->m_parameters.end() )
		throw exception("gts::http::request::parameter: key '{}' does not exist.", key);
	return it->second;
}

value request::parameter_or(const std::string &key, const value &deft_value) const
{
	auto it = m_impl->m_parameters.find(key);
	return it == m_impl->m_parameters.end()? deft_value : it->second;
}

value request::parameter_or(const std::string &key, value &&deft_value) const
{
	auto it = m_impl->m_parameters.find(key);
	return it == m_impl->m_parameters.end()? std::move(deft_value) : it->second;
}

const value &request::header(const std::string &key) const
{
	auto it = m_impl->m_headers.find(key);
	if( it == m_impl->m_headers.end() )
		throw exception("gts::http::request::header: key '{}' does not exist.", key);
	return it->second;
}

value request::header_or(const std::string &key, const value &deft_value) const
{
	auto it = m_impl->m_headers.find(key);
	return it == m_impl->m_headers.end()? deft_value : it->second;
}

value request::header_or(const std::string &key, value &&deft_value) const
{
	auto it = m_impl->m_headers.find(key);
	return it == m_impl->m_headers.end()? std::move(deft_value) : it->second;
}

const value &request::cookie(const std::string &key) const
{
	auto it = m_impl->m_cookies.find(key);
	if( it == m_impl->m_cookies.end() )
		throw exception("gts::http::request::cookie: key '{}' does not exist.", key);
	return it->second;
}

value request::cookie_or(const std::string &key, const value &deft_value) const
{
	auto it = m_impl->m_cookies.find(key);
	return it == m_impl->m_cookies.end()? deft_value : it->second;
}

value request::cookie_or(const std::string &key, value &&deft_value) const
{
	auto it = m_impl->m_cookies.find(key);
	return it == m_impl->m_cookies.end()? std::move(deft_value) : it->second;
}

using rb_status = request_impl::rb_status;

std::string request::read_body(std::error_code &error, std::size_t size)
{
	assert(m_impl);
	std::string result;
	error = std::error_code();

	if( m_impl->m_rb_status == rb_status::finished )
		return result;

	else if( size == 0 )
		size = m_impl->tcp_ip_buffer_size();

	char *buf = new char[size] {0};
	if( m_impl->m_rb_status == rb_status::length )
		size = m_impl->read_body_length_mode(error, buf, size);
	else
		size = m_impl->read_body_chunked_mode(error, buf, size);

	result = std::string(buf, size);
	delete[] buf;
	return result;
}

std::size_t request::read_body(std::error_code &error, void *buf, std::size_t size)
{
	assert(m_impl);
	error = std::error_code();

	if( size == 0 )
	{
		log_warning("request::read_body: size is 0.");
		return 0;
	}
	else if( m_impl->m_rb_status == rb_status::finished )
		return 0;

	else if( m_impl->m_rb_status == rb_status::length )
		size = m_impl->read_body_length_mode(error, buf, size);
	else
		size = m_impl->read_body_chunked_mode(error, buf, size);
	return size;
}

bool request::save_file(const std::string &_file_name, asio::error_code &error)
{
	if( _file_name.empty() )
	{
		log_error("request::save_file: file_name is empty.");
		error = std::make_error_code(std::errc::invalid_argument);
		return false;
	}

	auto file_name = app::absolute_path(_file_name);
	std::fstream file(file_name, std::ios_base::out);

	if( not file.is_open() )
	{
		error = std::make_error_code(static_cast<std::errc>(errno));
		return false;
	}

	auto tcp_buf_size = m_impl->tcp_ip_buffer_size();
	char *buf = new char[tcp_buf_size] {0};

	while( can_read_body() )
	{
		auto res = read_body(error, buf, tcp_buf_size);
		if( error )
		{
			file.close();
			delete[] buf;
			return false;
		}
		file.write(buf, res);
	}
	file.close();
	delete[] buf;
	return true;
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

bool request::can_read_body() const
{
	assert(m_impl);
	return m_impl->m_rb_status != rb_status::finished;
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
	if( m_impl == nullptr )
		return false;
	else if( m_impl->m_version.empty() or m_impl->m_path.empty() )
		return false;

	if( m_impl->m_method == GET )
	{
		auto it = m_impl->m_headers.find("content-length");
		if( it != m_impl->m_headers.end() )
		{
			if( it->second.get<std::size_t>() > 1024 )
				return false;
		}
	}
	return true;
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

void request::set_cookie_session_id(const std::string &id)
{
	m_impl->m_cookies["session_id"] = id;
	if( m_impl->m_response )
		m_impl->m_response->set_cookie("session_id", id);
}

}} //namespace gts::http
