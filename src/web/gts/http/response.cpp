#include "response.h"
#include "gts/web_global.h"

#include "service/service_io.h"
#include "gts/http/request.h"
#include "gts/tcp_socket.h"
#include "gts/mime_type.h"
#include "gts/algorithm.h"
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
	request &m_request;
	status m_status = hs_ok;

	headers m_headers {
		{ "content-type", "text/plain; charset=utf-8" }
	};
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

response::response(response &&other) :
	m_impl(other.m_impl)
{
	assert(other.m_impl);
	other.m_impl = nullptr;
}

response::~response()
{
	if( m_impl )
		delete m_impl;
}

response &response::set_status(int status)
{
	assert(m_impl);
	m_impl->m_status = static_cast<http::status>(status);
	return *this;
}

response &response::set_header(const std::string &key, const std::string &value)
{
	assert(m_impl);
	auto res = m_impl->m_headers.emplace(key, value);
	if( res.second == false and res.first != m_impl->m_headers.end() )
		res.first->second = value;
	return *this;
}

response &response::set_header(const std::string &key, std::string &&value)
{
	assert(m_impl);
	auto res = m_impl->m_headers.emplace(key, std::move(value));
	if( res.second == false and res.first != m_impl->m_headers.end() )
		res.first->second = std::move(value);
	return *this;
}

std::string response::version() const
{
	assert(m_impl);
	return m_impl->m_request.version();
}

const headers &response::headers() const
{
	assert(m_impl);
	return m_impl->m_headers;
}

status response::status() const
{
	assert(m_impl);
	return m_impl->m_status;
}

static std::function<void(response&)> g_write_default {nullptr};

response &response::write_default()
{
	assert(m_impl);
	if( m_impl->m_headers_writed )
	{
		log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return *this;
	}
	m_impl->m_headers_writed = true;

	if( g_write_default )
	{
		g_write_default(*this);
		return *this;
	}
	auto result = fmt::format("HTTP/{} {} {}\r\n", version(), static_cast<int>(m_impl->m_status),
							  status_description(m_impl->m_status));

	auto body = fmt::format("{} ({})", http::status_description(status()), status());
	set_header("content-length", body.size());

	for(auto &header : m_impl->m_headers)
		result += header.first + ": " + header.second + "\r\n";

	socket().write_some(result + "\r\n");
	socket().write_some(body);
	return *this;
}

response &response::write(std::size_t size, const void *body)
{
	assert(m_impl);
	if( m_impl->m_headers_writed )
	{
		log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return *this;
	}
	m_impl->m_headers_writed = true;

	auto result = fmt::format("HTTP/{} {} {}\r\n", version(), static_cast<int>(m_impl->m_status),
							  status_description(m_impl->m_status));

	for(auto &header : m_impl->m_headers)
		result += header.first + ": " + header.second + "\r\n";

	if( m_impl->m_headers.find("content-length") == m_impl->m_headers.end() )
		result += fmt::format("content-length: {}\r\n", size);

	socket().write_some(result + "\r\n");
	if( size > 0 )
		socket().write_some(body, size);
	return *this;
}

response &response::write_body(std::size_t size, const void *body)
{
	socket().write_some(body, size);
	return *this;
}

static std::string absolute_path(const std::string &path)
{
	auto result = path;
#ifdef _WINDOWS
	// ......
#else
	if( not starts_with(result, "/") )
	{
		if( starts_with(result, "~/") )
		{
			auto tmp = getenv("HOME");
			if( tmp == nullptr )
				log_fatal("System environment 'HOME' is null.");

			std::string home(tmp);
			if( not ends_with(home, "/") )
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
				log_info("The resource '{}' not exist.", m_file_name);
				m_response.write_default(http::hs_not_found);
				return false;
			}
		}

		m_file.open(m_file_name);
		if( not m_file.is_open() )
		{
			log_warning("Can't open resource '{}'.", m_file_name);
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
		auto it = headers.find("content-range");
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

		/*
			[size]\r\n
			[content]\r\n
			[size]\r\n
			[content]\r\n
			0\r\n
			\r\n
		*/
		it = headers.find("transfer-coding");
		if( it != headers.end() and it->second == "chunked" )
		{

			return m_file.close();
		}

		default_transfer();
		m_file.close();
	}

public:
	void default_transfer()
	{
		auto mime_type = get_mime_type(m_file_name);
		log_debug("resource mime-type: {}.", mime_type);

		auto file_size = fs::file_size(m_file_name);
		if( file_size == 0 )
			return ;

		m_response.set_header("content-length", file_size)
				  .set_header("content-type"  , mime_type)
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
		auto range_list = string_split(range_list_str, ",");
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

			m_response.set_header("accept-ranges" , "bytes")
					  .set_header("content-type"  , mime_type)
					  .set_header("content-length", range_value.size)
					  .set_header("content-range" , "{}-{}/{}", range_value.begin, range_value.end, range_value.size);

			return send_range("", "", range_value_queue);
		} // if( rangeList.size() == 1 )

		auto boundary = fmt::format("{}_{}", __func__, duration_cast<milliseconds>
									(system_clock::now().time_since_epoch()).count());

		m_response.set_header("content-type", "multipart/byteranges; boundary=" + boundary);

		auto ct_line = "content-type: " + mime_type;
		std::size_t content_length = 0;

		for(auto &str : range_list)
		{
			range_value_queue.emplace_back();
			auto &range_value = range_value_queue.back();

			if( range_parsing(trimmed(str), range_value.begin, range_value.end, range_value.size) == false )
			{
				m_response.write_default(http::hs_range_not_satisfiable);
				return ;
			}

			range_value.cr_line = fmt::format("content-range: bytes {}-{}/{}",
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

		m_response.set_header("content-length", content_length)
				  .set_header("accept-ranges" , "bytes");
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
		auto list = string_split(range_str, '-', false);

		if( list.size() > 2 )
		{
			log_debug("Range format error: {}.", range_str);
			return false;
		}

		auto file_size = fs::file_size(m_file_name);

		if( list[0].empty() )
		{
			if( list[1].empty() )
			{
				log_debug("Range format error.");
				return false;
			}

			std::sscanf(list[1].c_str(), "%zu", &size);
			if( size == 0 or size > file_size )
			{
				log_debug("Range size is invalid.");
				return false;
			}

			end   = file_size - 1;
			begin = file_size - size;
		}

		else if( list[1].empty() )
		{
			if( list[0].empty() )
			{
				log_debug("Range format error.");
				return false;
			}

			std::sscanf(list[0].c_str(), "%zu", &begin);
			end = file_size - 1;

			if( begin > end )
			{
				log_debug("Range is invalid.");
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
				log_debug("Range is invalid.");
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
	assert(m_impl);
	if( m_impl->m_headers_writed )
	{
		log_warning("The http protocol header is sent repeatedly. (auto ignore)");
		return *this;
	}
	file_transfer(m_impl->m_request, *this, file_name).call();
	return *this;
}

response &response::write_file(const std::string &file_name, const std::string &range_http_value)
{
	assert(m_impl);
	if( m_impl->m_headers_writed )
	{
		log_warning("The http protocol header is sent repeatedly. (auto ignore)");
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
		str += fmt::format("{}-{},", range.begin, range.end);

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
	default: log_fatal("Invalid redirect type.");
	}
	return set_header("location", url).write();
}

response &response::unset_header(const std::string &key)
{
	assert(m_impl);
	m_impl->m_headers.erase(key);
	return *this;
}

void response::close(bool force)
{
	socket().close(force);
}

const tcp_socket &response::socket() const
{
	assert(m_impl);
	return m_impl->m_request.socket();
}

tcp_socket &response::socket()
{
	assert(m_impl);
	return m_impl->m_request.socket();
}

response &response::operator=(response &&other)
{
	assert(other.m_impl);
	if( m_impl )
		delete m_impl;

	m_impl = other.m_impl;
	other.m_impl = nullptr;
	return *this;
}

void response::set_default_write(std::function<void(response&)> func)
{
	g_write_default = func;
}

}} //namespace gts::http
