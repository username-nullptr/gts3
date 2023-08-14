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

request::~request()
{
	if( m_impl )
		delete m_impl;
}

method request::method() const
{
	return m_impl->m_method;
}

std::string request::version() const
{
	return m_impl->m_version;
}

std::string request::path() const
{
	return m_impl->m_path;
}

std::string request::parameters_string() const
{
	return m_impl->m_parameters_string;
}

const parameters &request::parameters() const
{
	return m_impl->m_parameters;
}

const headers &request::headers() const
{
	return m_impl->m_headers;
}

const basic_cookies &request::cookies() const
{
	return m_impl->m_cookies;
}

bool request::parameters_contains(const std::string &key) const
{
	return m_impl->m_parameters.find(key) != m_impl->m_parameters.end();
}

const value &request::parameter(const std::string &key) const
{
	auto it = m_impl->m_parameters.find(key);
	if( it == m_impl->m_parameters.end() )
		throw exception("gts::http::request::parameter: key '{}' does not exist.", key);
	return it->second;
}

value request::parameter_or(const std::string &key, value deft_value) const
{
	auto it = m_impl->m_parameters.find(key);
	return it == m_impl->m_parameters.end()? std::move(deft_value) : it->second;
}

bool request::headers_contains(const std::string &key) const
{
	return m_impl->m_headers.find(key) != m_impl->m_headers.end();
}

const value &request::header(const std::string &key) const
{
	auto it = m_impl->m_headers.find(key);
	if( it == m_impl->m_headers.end() )
		throw exception("gts::http::request::header: key '{}' does not exist.", key);
	return it->second;
}

value request::header_or(const std::string &key, value deft_value) const
{
	auto it = m_impl->m_headers.find(key);
	return it == m_impl->m_headers.end()? std::move(deft_value) : it->second;
}

bool request::cookies_contains(const std::string &key) const
{
	return m_impl->m_cookies.find(key) != m_impl->m_cookies.end();
}

const value &request::cookie(const std::string &key) const
{
	auto it = m_impl->m_cookies.find(key);
	if( it == m_impl->m_cookies.end() )
		throw exception("gts::http::request::cookie: key '{}' does not exist.", key);
	return it->second;
}

value request::cookie_or(const std::string &key, value deft_value) const
{
	auto it = m_impl->m_cookies.find(key);
	return it == m_impl->m_cookies.end()? std::move(deft_value) : it->second;
}

using rb_status = request_impl::rb_status;

std::string request::read_body(std::error_code &error, std::size_t size)
{
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
	error = std::error_code();
	if( size == 0 )
	{
		gts_log_warning("request::read_body: size is 0.");
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

std::string request::read_all_body(std::error_code &error)
{
    std::string result;
    auto tcp_buf_size = m_impl->tcp_ip_buffer_size();
	char *buf = new char[65536] {0};

	while( can_read_body() )
	{
		auto res = read_body(error, buf, tcp_buf_size);
		if( error )
		{
			delete[] buf;
			return result;
		}
		else if( res == 0 )
			continue;
		result.append(buf, res);
	}
	delete[] buf;
	return result;
}

bool request::save_file(const std::string &_file_name, asio::error_code &error, std::size_t begin)
{
	if( _file_name.empty() )
	{
		gts_log_error("request::save_file: file_name is empty.");
		error = std::make_error_code(std::errc::invalid_argument);
		return false;
	}
	auto file_name = app::absolute_path(_file_name);
	if( begin > 0 and not fs::exists(file_name) )
	{
		error = std::make_error_code(std::errc::no_such_file_or_directory);
		return false;
	}
	std::fstream file(file_name, std::ios_base::out);
	if( not file.is_open() )
	{
		error = std::make_error_code(static_cast<std::errc>(errno));
		return false;
	}
	file.seekp(begin);
	auto tcp_buf_size = m_impl->tcp_ip_buffer_size();
	char *buf = new char[65536] {0};

	while( can_read_body() )
	{
		auto res = read_body(error, buf, tcp_buf_size);
		if( error )
		{
			file.close();
			delete[] buf;
			return false;
		}
		else if( res == 0 )
			continue;
		file.write(buf, res);
	}
	file.close();
	delete[] buf;
	return true;
}

bool request::keep_alive() const
{
	return m_impl->m_keep_alive;
}

bool request::support_gzip() const
{
	return m_impl->m_support_gzip;
}

bool request::can_read_body() const
{
	return m_impl->m_rb_status != rb_status::finished;
}

bool request::is_valid() const
{
	if( m_impl == nullptr )
		return false;
	else if( m_impl->m_version.empty() or m_impl->m_path.empty() )
		return false;

	if( m_impl->m_method == GET )
	{
		auto it = m_impl->m_headers.find(header::content_length);
		if( it != m_impl->m_headers.end() )
		{
			if( it->second.get<std::size_t>() > 1024 )
				return false;
		}
	}
	return true;
}

tcp::endpoint request::remote_endpoint() const
{
	return m_impl->m_socket->remote_endpoint();
}

tcp::endpoint request::local_endpoint() const
{
	return m_impl->m_socket->local_endpoint();
}

void request::set_cookie_session_id(std::string id)
{
	if( m_impl->m_response )
	{
		m_impl->m_cookies["session_id"] = id;
		m_impl->m_response->set_cookie("session_id", std::move(id));
	}
	else
		m_impl->m_cookies["session_id"] = std::move(id);
}

}} //namespace gts::http
