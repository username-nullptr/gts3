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

session_ptr request::session() const
{
	auto it = m_impl->m_cookies.find("session_id");
	if( it == m_impl->m_cookies.end() )
		return session_ptr();

	auto ptr = http::session::get(it->second);
	if( ptr )
	{
		if( ptr->is_valid() )
			ptr->expand();
		return ptr;
	}
	return session_ptr();
}

session_ptr request::session(bool create)
{
	auto it = m_impl->m_cookies.find("session_id");
	if( it == m_impl->m_cookies.end() )
	{
		if( not create )
			return session_ptr();
		return m_impl->create_session();
	}
	auto ptr = http::session::get(it->second);
	if( ptr )
	{
		if( ptr->is_valid() )
			ptr->expand();
		return ptr;
	}
	else if( create )
		return m_impl->create_session();
	return session_ptr();
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

std::string request::read_body(std::error_code &error, std::size_t size)
{
	assert(m_impl);
	std::size_t content_length = 0;
	auto &headers = this->headers();

	std::string result;
	auto it = headers.find("content-length");

	if( it != headers.end() )
	{
		content_length = it->second.get<std::size_t>();
		if( m_impl->m_rclenght >= content_length )
			return 0;

		content_length -= m_impl->m_rclenght;
		if( size == 0 )
		{
			if( not m_impl->m_body.empty() )
			{
				result += m_impl->m_body;
				m_impl->m_body.clear();
			}
			if( result.size() > content_length )
				result.erase(content_length);
			else if( result.size() < content_length )
			{
				auto buf_size = m_impl->tcp_ip_buffer_size();
				char *buf = new char[buf_size] {0};

				for(;;)
				{
					auto res = m_impl->m_socket->read_some(buf, buf_size, error);
					if( error )
					{
						delete[] buf;
						return 0;
					}
					else if( res > 0 )
						result += std::string(buf, res);
				}
				delete[] buf;
			}
			m_impl->m_rclenght += result.size();
			return result;
		}
	}
	else if( size == 0 )
		size = m_impl->tcp_ip_buffer_size();

	if( not m_impl->m_body.empty() )
	{
		if( size < m_impl->m_body.size() )
		{
			result = m_impl->m_body.substr(0, size);
			m_impl->m_body.erase(0, size);
			m_impl->m_rclenght += size;
			return result;
		}
		else if( size == m_impl->m_body.size() )
		{
			result = std::move(m_impl->m_body);
			m_impl->m_rclenght += size;
			return result;
		}
		result = std::move(m_impl->m_body);
		size -= result.size();

		if( content_length == 0 )
			return result;
	}
	char *buf = new char[size] {0};
	auto res = m_impl->m_socket->read_some(buf, size, error);
	if( error )
	{
		delete[] buf;
		return 0;
	}
	else if( res > 0 )
	{
		result += std::string(buf, res);
		m_impl->m_rclenght += res;
	}
	delete[] buf;
	return result;
}

std::size_t request::read_body(std::error_code &error, void *buf, std::size_t size)
{
	assert(m_impl);
	if( size == 0 )
	{
		log_warning("request::read_body: size is 0.");
		return 0;
	}

	std::size_t content_length = 0;
	auto &headers = this->headers();

	auto it = headers.find("content-length");
	if( it != headers.end() )
	{
		content_length = it->second.get<std::size_t>();
		if( m_impl->m_rclenght >= content_length )
			return 0;

		content_length -= m_impl->m_rclenght;
		if( content_length < size )
			size = content_length;
		if( size == 0 )
			return 0;
	}

	std::size_t offset = 0;
	if( not m_impl->m_body.empty() )
	{
		if( size < m_impl->m_body.size() )
		{
			memcpy(buf, m_impl->m_body.c_str(), size);
			m_impl->m_body.erase(0, size);
			m_impl->m_rclenght += size;
			return size;
		}
		memcpy(buf, m_impl->m_body.c_str(), m_impl->m_body.size());
		if( size == m_impl->m_body.size() )
		{
			m_impl->m_body.clear();
			m_impl->m_rclenght += size;
			return size;
		}
		size -= m_impl->m_body.size();
		offset += m_impl->m_body.size();
		m_impl->m_body.clear();

		if( content_length == 0 )
			return offset;
	}
	auto res = m_impl->m_socket->read_some(static_cast<char*>(buf) + offset, size, error);
	if( error )
		return 0;

	auto result =  offset + (res > 0 ? res : 0);
	m_impl->m_rclenght += result;
	return result;
}

bool request::save_file(const std::string &_file_name, asio::error_code &error)
{
	if( m_impl->m_rclenght > 0 )
	{
		log_error("request::save_file: 'read_body' has been called.");
		error = std::make_error_code(std::errc::invalid_argument);
		return false;
	}
	else if( _file_name.empty() )
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

	auto &headers = this->headers();
	auto it = headers.find("content-length");

	if( it != headers.end() )
	{
		auto tcp_buf_size = m_impl->tcp_ip_buffer_size();
		auto asize = it->second.get<std::size_t>();
		char *buf = new char[tcp_buf_size] {0};

		while( asize > 0 )
		{
			auto res = read_body(error, buf, asize > tcp_buf_size ? tcp_buf_size : asize);
			if( error )
			{
				delete[] buf;
				return false;
			}
			asize -= res;
			file.write(buf, res);
		}
		file.close();
		delete[] buf;
		return true;
	}
	else if( version() == "1.1" )
	{
		it = headers.find("transfer-coding");
		if( it == headers.end() or it->second != "chunked" )
		{
			log_warning("http protocol format error.");
			file.close();
			return false;
		}

		auto tcp_buf_size = m_impl->tcp_ip_buffer_size();
		char *buf = new char[tcp_buf_size] {0};
		std::string abuf;

		enum class status
		{
			wait_size,
			wait_content,
			wait_headers
		}
		_status = status::wait_size;

		for(;;)
		{
			auto res = read_body(error, buf, tcp_buf_size);
			if( error )
			{
				file.close();
				delete[] buf;
				return false;
			}

			abuf.append(buf, res);
			std::size_t _size = 0;

			auto lambda_reset = [&](const char *msg)
			{
				log_warning(msg);
				error = std::make_error_code(std::errc::wrong_protocol_type);
				file.close();
				fs::remove(file_name);
				delete[] buf;
			};
			while( not abuf.empty() )
			{
				auto pos = abuf.find("\r\n");
				if( pos == abuf.npos )
				{
					if( abuf.size() >= 1024 )
					{
						lambda_reset("Http protocol format error.");
						return false;
					}
					break;
				}

				auto line_buf = abuf.substr(0, pos + 2);
				abuf.erase(0, pos + 2);

				if( _status == status::wait_size )
				{
					line_buf.erase(pos);
					auto pos = line_buf.find(";");
					if( pos != line_buf.npos )
						line_buf.erase(pos);

					if( line_buf.size() > 16 )
					{
						lambda_reset("Http protocol format error.");
						return false;
					}
					try {
						_size = static_cast<std::size_t>(std::stoull(line_buf, nullptr, 16));
					} catch(...) {
						lambda_reset("Http protocol format error.");
						return false;
					}
					_status = _size == 0 ? status::wait_headers : status::wait_content;
				}
				else if( _status == status::wait_content )
				{
					line_buf.erase(pos);
					if( _size < line_buf.size() )
						_size = line_buf.size();
					else
						_status = status::wait_size;
					file.write(line_buf.c_str(), _size);
				}
				else if( _status == status::wait_headers )
				{
					if( line_buf == "\r\n" )
					{
						file.close();
						delete[] buf;
						return true;
					}
					auto colon_index = line_buf.find(':');
					if( colon_index == line_buf.npos )
					{
						lambda_reset("Invalid header line.");
						return false;
					}
					auto header_key = to_lower(trimmed(line_buf.substr(0, colon_index)));
					auto header_value = from_percent_encoding(trimmed(line_buf.substr(colon_index + 1)));
					m_impl->m_headers[header_key] = header_value;
				}
			}
		}
	}
	log_error("request::save_file: Insufficient 'http' condition.\n"
			  "    version: {}\n"
			  "    content-length: not found\n"
			  "    transfer-coding: chunked: not found", version());
	error = std::make_error_code(std::errc::invalid_argument);
	file.close();
	return false;
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

}} //namespace gts::http
