#include "session/request_impl.h"
#include "gts/algorithm.h"
#include "gts/log.h"
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
	}
	auto res = m_impl->m_socket->read_some(static_cast<char*>(buf) + offset, size, error);
	if( error )
		return 0;

	auto result =  offset + (res > 0 ? res : 0);
	m_impl->m_rclenght += result;
	return result;
}

bool request::save_file(const std::string &file_name, asio::error_code &error)
{
	if( m_impl->m_rclenght > 0 )
	{
		log_error("request::save_file: 'read_body' has been called.");
		error = std::make_error_code(std::errc::invalid_argument);
		return false;
	}
	else if( file_name.empty() )
	{
		log_error("request::save_file: file_name is empty.");
		error = std::make_error_code(std::errc::invalid_argument);
		return false;
	}

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
		if( it != headers.end() and it->second == "chunked" )
		{
			auto tcp_buf_size = m_impl->tcp_ip_buffer_size();
			char *buf = new char[tcp_buf_size] {0};
			std::string abuf;
			bool mode = false; // false:size true:content

			for(;;)
			{
				auto res = read_body(error, buf, tcp_buf_size);
				if( error )
				{
					delete[] buf;
					return false;
				}

				abuf.append(buf, res);
				auto list = string_split(abuf, "\r\n");
				std::size_t _size = 0;

				for(auto it2=list.begin(); it2!=list.end(); ++it2)
				{
					auto &line = *it2;
					if( mode )
					{
						if( _size < line.size() )
							_size = line.size();
						else
							mode = false;

						file.write(line.c_str(), _size);
						continue;
					}

					auto pos = line.find(";");
					if( pos != line.npos )
						line.erase(pos);

					std::sscanf(line.c_str(), "%zu", &_size);
					if( _size > 0 )
					{
						mode = true;
						continue;
					}

					for(++it2; it2!=list.end(); ++it2)
					{
						pos = it2->find(":");
						if( pos == std::string::npos )
							m_impl->m_headers.emplace(to_lower(trimmed(*it2)), "");
						else
							m_impl->m_headers.emplace(to_lower(trimmed(it2->substr(0,pos))), to_lower(trimmed(it2->substr(pos+1))));
					}
					delete[] buf;
					return true;
				}
			}
			log_warning("http protocol format error.");
			file.close();
			delete[] buf;
			return true;
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
