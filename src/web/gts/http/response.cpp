#include "response.h"
#include "service/service_io.h"

#include "gts/http/request.h"
#include "gts/tcp_socket.h"
#include "gts/mime_type.h"
#include "gts/algorithm.h"
#include "gts/exception.h"
#include "gts/log.h"

#include <cppfilesystem>
#include <fstream>
#include <cassert>

namespace gts { namespace http
{

class GTS_DECL_HIDDEN response_impl
{
public:
	explicit response_impl(http::request &request) :
		m_request(request) {}

public:
	std::size_t tcp_ip_buffer_size() const
	{
		tcp::socket::send_buffer_size attr;
		m_request.socket().get_option(attr);
		return attr.value();
	}

public:
	void serialize_headers_and_cookies(std::string &result)
	{
		m_headers.erase("set-cookie");
		for(auto &header : m_headers)
			result += header.first + ": " + header.second + "\r\n";

		for(auto &cookie_pair : m_cookies)
		{
			auto &cookie = cookie_pair.second;
			result += "set-cookie: " + cookie_pair.first + "=" + cookie + ";";

			for(auto &attr_pair : cookie.attributes())
				result += attr_pair.first + "=" + attr_pair.second + ";";

			result.pop_back();
			result += "\r\n";
		}
	}

public:
	request &m_request;
	status m_status = hs_ok;

	cookies m_cookies;
	headers m_headers {
		{ header::content_type, "text/plain; charset=utf-8" }
	};
	value_list m_chunk_attributes;
	bool m_headers_writed = false;
};

/*---------------------------------------------------------------------------------------------------------------*/

response::response(http::request &request, http::status status) :
	m_impl(new response_impl(request))
{
	m_impl->m_status = status;
}

response::response(http::request &request, const http::headers &headers, http::status status) :
	response(request, status)
{
	m_impl->m_headers = headers;
}

response::~response()
{
	if( m_impl == nullptr )
		return ;
	else if( not m_impl->m_headers_writed )
		write_default();
	delete m_impl;
}

response &response::set_status(int status)
{
	m_impl->m_status = static_cast<http::status>(status);
	return *this;
}

response &response::set_headers(const http::headers &headers)
{
	for(auto &p : headers)
		set_header(p.first, p.second);
	return *this;
}

response &response::set_cookies(const http::cookies &cookies)
{
	for(auto &p : cookies)
		set_cookie(p.first, p.second);
	return *this;
}

response &response::set_header(std::string key, http::value value)
{
	m_impl->m_headers[std::move(key)] = std::move(value);
	return *this;
}

response &response::set_cookie(std::string key, http::cookie cookie)
{
	m_impl->m_cookies[std::move(key)] = std::move(cookie);
	return *this;
}

std::string response::version() const
{
	return m_impl->m_request.version();
}

status response::status() const
{
	return m_impl->m_status;
}

const headers &response::headers() const
{
	return m_impl->m_headers;
}

headers &response::headers()
{
	return m_impl->m_headers;
}

const http::cookies &response::cookies() const
{
	return m_impl->m_cookies;
}

http::cookies &response::cookies()
{
	return m_impl->m_cookies;
}

bool response::headers_contains(const std::string &key) const
{
	return m_impl->m_headers.find(key) != m_impl->m_headers.end();
}

value &response::header(const std::string &key)
{
	auto it = m_impl->m_headers.find(key);
	if( it == m_impl->m_headers.end() )
		throw exception("gts::http::response::header: key '{}' does not exist.", key);
	return it->second;
}

const value &response::header(const std::string &key) const
{
	auto it = m_impl->m_headers.find(key);
	if( it == m_impl->m_headers.end() )
		throw exception("gts::http::response::header: key '{}' does not exist.", key);
	return it->second;
}

value response::header_or(const std::string &key, value deft_value) const
{
	auto it = m_impl->m_headers.find(key);
	return it == m_impl->m_headers.end()? std::move(deft_value) : it->second;
}

bool response::cookies_contains(const std::string &key) const
{
	return m_impl->m_cookies.find(key) != m_impl->m_cookies.end();
}

cookie &response::cookie(const std::string &key)
{
	auto it = m_impl->m_cookies.find(key);
	if( it == m_impl->m_cookies.end() )
		throw exception("gts::http::response::cookie: key '{}' does not exist.", key);
	return it->second;
}

const cookie &response::cookie(const std::string &key) const
{
	auto it = m_impl->m_cookies.find(key);
	if( it == m_impl->m_cookies.end() )
		throw exception("gts::http::response::cookie: key '{}' does not exist.", key);
	return it->second;
}

cookie response::cookie_or(const std::string &key, http::cookie deft_value) const
{
	auto it = m_impl->m_cookies.find(key);
	return it == m_impl->m_cookies.end()? std::move(deft_value) : it->second;
}

static std::function<void(response&)> g_write_default {nullptr};

response &response::write_default()
{
	if( m_impl->m_headers_writed )
	{
		gts_log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return *this;
	}
	m_impl->m_headers_writed = true;

	if( g_write_default )
	{
		g_write_default(*this);
		return *this;
	}
	auto body = fmt::format("{} ({})", http::status_description(status()), status());
	set_header(header::content_length, body.size());

	auto result = fmt::format("HTTP/{} {} {}\r\n", version(), static_cast<int>(m_impl->m_status),
							  status_description(m_impl->m_status));
	m_impl->serialize_headers_and_cookies(result);

	socket().write_some(result + "\r\n");
	socket().write_some(body);
	return *this;
}

response &response::write(std::size_t size, const void *body)
{
	if( m_impl->m_headers_writed )
	{
		gts_log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return *this;
	}
	m_impl->m_headers_writed = true;

	auto result = fmt::format("HTTP/{} {} {}\r\n", version(), static_cast<int>(m_impl->m_status),
							  status_description(m_impl->m_status));
	m_impl->serialize_headers_and_cookies(result);

	if( m_impl->m_headers.find(header::content_length) == m_impl->m_headers.end() )
		result += fmt::format("{}: {}\r\n", header::content_length, size);

	socket().write_some(result + "\r\n");
	if( size > 0 )
		socket().write_some(body, size);
	return *this;
}

response &response::set_chunk_attribute(value attribute)
{
	m_impl->m_chunk_attributes.emplace_back(std::move(attribute));
	return *this;
}

response &response::set_chunk_attributes(value_list attributes)
{
	for(auto &value : attributes)
		set_chunk_attribute(std::move(value));
	return *this;
}

response &response::write_body(std::size_t size, const void *body)
{
	if( size == 0 )
		return *this;
	else if( not m_impl->m_headers_writed )
		write();

	auto &sock = socket();
	if( version() == "1.1" )
	{
		auto it = m_impl->m_headers.find(header::transfer_coding);
		if( it != m_impl->m_headers.end() and str_to_lower(it->second) == "chunked" )
		{
			if( m_impl->m_chunk_attributes.empty() )
				sock.write_some(fmt::format("{:X}\r\n", size));
			else
			{
				std::string attributes;
				for(auto &attr : m_impl->m_chunk_attributes)
					attributes += attr + ";";

				m_impl->m_chunk_attributes.clear();
				attributes.pop_back();
				sock.write_some(fmt::format("{:X}; {}\r\n", size, attributes));
			}
			sock.write_some(body, size);
			sock.write_some("\r\n");
		}
		else
			sock.write_some(body, size);
	}
	else
		sock.write_some(body, size);
	return *this;
}

response &response::chunk_end(const http::headers &headers)
{
	if( version() != "1.1" )
		throw exception("gts::response::chunk_end: Only HTTP/1.1 supports 'Transfer-Coding: chunked'.");

	auto it = m_impl->m_headers.find(header::transfer_coding);
	if( it == m_impl->m_headers.end() or str_to_lower(it->second) != "chunked" )
		throw exception("gts::response::write_file: 'Transfer-Coding: chunked' not set.");

	auto &sock = socket();
	sock.write_some("0\r\n");

	std::string buf;
	for(auto &header : headers)
		buf += header.first + ": " + header.second + "\r\n";

	sock.write_some(buf + "\r\n");
	return *this;
}

static std::string absolute_path(const std::string &path)
{
	auto result = path;
#ifdef _WINDOWS
	// ......
#else
	if( not str_starts_with(result, "/") )
	{
		if( str_starts_with(result, "~/") )
		{
			auto tmp = getenv("HOME");
			if( tmp == nullptr )
				gts_log_fatal("System environment 'HOME' is null.");

			std::string home(tmp);
			if( not str_ends_with(home, "/") )
				home += "/";
			result = home + result.erase(0,2);
		}
		else
			result = web::resource_root() + result;
	}
#endif //unix
	return result;
}

class GTS_DECL_HIDDEN file_transfer
{
	GTS_DISABLE_COPY_MOVE(file_transfer)

public:
	explicit file_transfer(http::request &request, http::response &response, const std::string &file_name) :
	  m_request(request), m_response(response), m_file_name(absolute_path(file_name)) {}

public:
	bool check()
	{
		if( not fs::exists(m_file_name) )
		{
			m_file_name += ".html";
			if( not fs::exists(m_file_name) )
			{
				gts_log_info("The resource '{}' not exist.", m_file_name);
				m_response.write_default(http::hs_not_found);
				return false;
			}
		}

		m_file.open(m_file_name);
		if( not m_file.is_open() )
		{
			gts_log_warning("Can't open resource '{}'.", m_file_name);
			m_response.write_default(http::hs_forbidden);
			return false;
		}
		return true;
	}

	void call()
	{
		if( check() == false )
			return ;
		auto &headers = m_request.headers();

		// Content-Range: bytes=x-y/z
		auto it = headers.find(header::content_range);
		if( it != headers.end() )
		{
			// TODO ......
			m_response.write_default(http::hs_not_implemented); //tmp
			return m_file.close();
		}

		// Range: bytes=x-y, m-n, i-j ...
		it = headers.find("range");
		if( it != headers.end() )
		{
			range_transfer(it->second);
			return m_file.close();
		}
		default_transfer();
		m_file.close();
	}

public:
	void default_transfer()
	{
		auto mime_type = get_mime_type(m_file_name);
		gts_log_debug("resource mime-type: {}.", mime_type);

		auto file_size = fs::file_size(m_file_name);
		if( file_size == 0 )
			return ;

		m_response.set_header(header::content_length, file_size)
				  .set_header(header::content_type  , mime_type)
				  .write();

		auto buf_size = tcp_ip_buffer_size();
		char *fr_buf = new char[buf_size] {0};

		for(;;)
		{
			std::size_t size = m_file.readsome(fr_buf, buf_size);
			if( size == 0 )
				break;

			m_response.write_body(size, fr_buf);
			if( size < buf_size )
				break;
		}
		delete[] fr_buf;
	}

	void range_transfer(const std::string &_range_str)
	{
		using namespace std::chrono;

		auto range_str = _range_str;
		for(long i=range_str.size(); i>0; i--)
		{
			if( range_str[i] == ' ' )
				range_str.erase(i,1);
		}
		if( range_str.empty() )
		{
			m_response.write_default(http::hs_bad_request);
			return ;
		}

		// bytes=x-y, m-n, i-j ...
		if( range_str.substr(0,6) != "bytes=" )
		{
			m_response.write_default(http::hs_range_not_satisfiable);
			return ;
		}

		// x-y, m-n, i-j ...
		auto range_list_str = range_str.substr(6);
		if( range_list_str.empty() )
		{
			m_response.write_default(http::hs_range_not_satisfiable);
			return ;
		}

		// (x-y) ( m-n) ( i-j) ...
		auto range_list = str_split(range_list_str, ",");
		auto mime_type = get_mime_type(m_file_name);

		m_response.set_status(http::hs_partial_content);
		std::list<range_value> range_value_queue;

		if( range_list.size() == 1 )
		{
			range_value_queue.emplace_back();
			auto &range_value = range_value_queue.back();

			if( range_parsing(range_list[0], range_value.begin, range_value.end, range_value.size) == false )
			{
				m_response.write_default(http::hs_range_not_satisfiable);
				return ;
			}

			m_response.set_header(header::accept_ranges , "bytes")
					  .set_header(header::content_type  , mime_type)
					  .set_header(header::content_length, range_value.size)
					  .set_header(header::content_range , "{}-{}/{}", range_value.begin, range_value.end, range_value.size);

			return send_range("", "", range_value_queue);
		} // if( rangeList.size() == 1 )

		auto boundary = fmt::format("{}_{}", __func__, duration_cast<milliseconds>
									(system_clock::now().time_since_epoch()).count());

		m_response.set_header(header::content_type, "multipart/byteranges; boundary=" + boundary);

		auto ct_line = fmt::format("{}: {}", header::content_type, mime_type);
		std::size_t content_length = 0;

		for(auto &str : range_list)
		{
			range_value_queue.emplace_back();
			auto &range_value = range_value_queue.back();

			if( range_parsing(str_trimmed(str), range_value.begin, range_value.end, range_value.size) == false )
			{
				m_response.write_default(http::hs_range_not_satisfiable);
				return ;
			}

			range_value.cr_line = fmt::format("{}: bytes {}-{}/{}", header::content_range,
											  range_value.begin, range_value.end, fs::file_size(m_file_name));
			/*
				--boundary<CR><LF>
				Content-Type: xxx<CR><LF>
				Content-Range: bytes 3-11/96<CR><LF>
				<CR><LF>
				012345678<CR><LF>
				--boundary<CR><LF>
				Content-Type: xxx<CR><LF>
				Content-Range: bytes 0-7/96<CR><LF>
				<CR><LF>
				01235467<CR><LF>
				--boundary--<CR><LF>
			*/
			content_length += 2 + boundary.size() + 2 +        // --boundary<CR><LF>
							  ct_line.size() + 2 +             // Content-Type: xxx<CR><LF>
							  range_value.cr_line.size() + 2 + // Content-Range: bytes 3-11/96<CR><LF>
							  2 +                              // <CR><LF>
							  range_value.size + 2;            // 012345678<CR><LF>
		}
		content_length += 2 + boundary.size() + 2 + 2;         // --boundary--<CR><LF>

		m_response.set_header(header::content_length, content_length)
				  .set_header(header::accept_ranges , "bytes");
		send_range(boundary, ct_line, range_value_queue);
	}

private:
	struct range_value
	{
		std::string cr_line;
		std::size_t begin, end, size;
	};

private:
	bool range_parsing(const std::string &range_str, std::size_t &begin, std::size_t &end, std::size_t &size)
	{
		auto list = str_split(range_str, '-', false);

		if( list.size() > 2 )
		{
			gts_log_debug("Range format error: {}.", range_str);
			return false;
		}

		auto file_size = fs::file_size(m_file_name);

		if( list[0].empty() )
		{
			if( list[1].empty() )
			{
				gts_log_debug("Range format error.");
				return false;
			}

			std::sscanf(list[1].c_str(), "%zu", &size);
			if( size == 0 or size > file_size )
			{
				gts_log_debug("Range size is invalid.");
				return false;
			}

			end   = file_size - 1;
			begin = file_size - size;
		}

		else if( list[1].empty() )
		{
			if( list[0].empty() )
			{
				gts_log_debug("Range format error.");
				return false;
			}

			std::sscanf(list[0].c_str(), "%zu", &begin);
			end = file_size - 1;

			if( begin > end )
			{
				gts_log_debug("Range is invalid.");
				return false;
			}
			size  = file_size - begin;
		}

		else
		{
			std::sscanf(list[0].c_str(), "%zu", &begin);
			std::sscanf(list[1].c_str(), "%zu", &end);

			if( begin > end or end >= file_size )
			{
				gts_log_debug("Range is invalid.");
				return false;
			}
			size  = end - begin + 1;
		}
		return true;
	}

	void send_range(const std::string &boundary, const std::string &ct_line, std::list<range_value> &range_value_queue)
	{
		assert(not range_value_queue.empty());
		m_response.write();

		auto buf_size = tcp_ip_buffer_size();
		bool flag = true;

		if( range_value_queue.size() == 1 )
		{
			auto &value = range_value_queue.back();
			m_file.seekg(value.begin, std::ios_base::beg);

			do {
				if( value.size <= buf_size )
				{
					auto buf = new char[value.size] {0};
					auto s = m_file.readsome(buf, value.size);

					m_response.write_body(s, buf);
					delete[] buf;
					flag = false;
				}
				else
				{
					auto buf = new char[buf_size] {0};
					auto s = m_file.readsome(buf, buf_size);

					m_response.write_body(s, buf);
					value.size -= buf_size;
					delete[] buf;
				}
			}
			while(flag);
			return ;
		}

		for(auto &value : range_value_queue)
		{
			m_response.write_body("--" + boundary + "\r\n" +
								  ct_line + "\r\n" +
								  value.cr_line + "\r\n" +
								  "\r\n");

			m_file.seekg(value.begin, std::ios_base::beg);
			do {
				if( value.size <= buf_size )
				{
					auto buf = new char[value.size + 2] {0};
					auto s = m_file.readsome(buf, value.size);

					buf[s + 0] = '\r';
					buf[s + 1] = '\n';

					m_response.write_body(s + 2, buf);
					delete[] buf;
					flag = false;
				}
				else
				{
					auto buf = new char[buf_size] {0};
					auto s = m_file.readsome(buf, buf_size);

					m_response.write_body(s, buf);
					value.size -= buf_size;
					delete[] buf;
				}
			}
			while(flag);
		}
		m_response.write_body("--" + boundary + "--\r\n");
	}

private:
	std::size_t tcp_ip_buffer_size() const
	{
		tcp::socket::send_buffer_size attr;
		m_request.socket().get_option(attr);
		return attr.value();
	}

private:
	http::request &m_request;
	http::response &m_response;
	std::string m_file_name;
	std::fstream m_file;
};

response &response::write_file(const std::string &file_name)
{
	if( m_impl->m_headers_writed )
	{
		gts_log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return *this;
	}
	file_transfer(m_impl->m_request, *this, file_name).call();
	return *this;
}

response &response::write_file(const std::string &file_name, const std::string &range_http_value)
{
	if( m_impl->m_headers_writed )
	{
		gts_log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return *this;
	}
	file_transfer ft(m_impl->m_request, *this, file_name);
	if( ft.check() )
		ft.range_transfer(range_http_value);
	return *this;
}

response &response::write_file(const std::string &file_name, const range_vector &vector)
{
	if( vector.empty() )
		return write_file(file_name);

	std::string str = "bytes=";
	for(auto &range : vector)
	{
		if( range.begin >= range.end )
			str += fmt::format("{}-{},", range.begin, range.end);
		else
			throw exception("gts::response::write_file: range error: begin > end.");
	}
	str.pop_back();
	return write_file(file_name, str);
}

response &response::redirect(const std::string &url, redirect_type type)
{
	switch(type)
	{
	case redirect_type::moved_permanently  : set_status(http::hs_moved_permanently ); break;
	case redirect_type::permanent_redirect : set_status(http::hs_permanent_redirect); break;
	case redirect_type::found              : set_status(http::hs_found             ); break;
	case redirect_type::see_other          : set_status(http::hs_see_other         ); break;
	case redirect_type::temporary_redirect : set_status(http::hs_temporary_redirect); break;
	case redirect_type::multiple_choices   : set_status(http::hs_multiple_choices  ); break;
	case redirect_type::not_modified       : set_status(http::hs_not_modified      ); break;
	default: gts_log_fatal("Invalid redirect type.");
	}
	return set_header(header::location, url).write();
}

response &response::unset_header(const std::string &key)
{
	m_impl->m_headers.erase(key);
	return *this;
}

response &response::unset_cookie(const std::string &key)
{
	m_impl->m_cookies.erase(key);
	return *this;
}

void response::close(bool force)
{
	socket().close(force);
}

const tcp_socket &response::socket() const
{
	return m_impl->m_request.socket();
}

tcp_socket &response::socket()
{
	return m_impl->m_request.socket();
}

void response::set_default_write(std::function<void(response&)> func)
{
	g_write_default = func;
}

bool response::is_writed() const
{
	return m_impl->m_headers_writed;
}

}} //namespace gts::http
