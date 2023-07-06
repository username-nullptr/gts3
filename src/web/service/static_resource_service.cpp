#include "static_resource_service.h"
#include "service/service_io.h"
#include "gts/algorithm.h"
#include "gts/mime_type.h"
#include "gts/log.h"

#include <cppfilesystem>

namespace gts { namespace web
{

static_resource_service::static_resource_service(service_io &sio) :
	m_sio(sio)
{

}

void static_resource_service::call()
{
	if( not fs::exists(m_sio.url_name) )
	{
		log_info("The resource '{}' not exist.", m_sio.url_name);
		return m_sio.return_to_null(http::hs_not_found);
	}

	m_file.open(m_sio.url_name);
	if( not m_file.is_open() )
	{
		log_warning("Can't open resource '{}'.", m_sio.url_name);
		return m_sio.return_to_null(http::hs_forbidden);
	}

	auto it = m_sio.request.headers.find("range");
	if( it == m_sio.request.headers.end() )
		default_transfer();

	else if( not it->second.empty() ) // Range: bytes=x-y, m-n, i-j ...
		range_transfer(it->second);
	else
		m_sio.return_to_null(http::hs_bad_request);

	m_file.close();
}

void static_resource_service::default_transfer()
{
	auto mime_type = get_mime_type(m_sio.url_name);
	log_debug("resource mime-type: {}.", mime_type);

	// ??????????????
//	if( mime_type == "application/gzip" or mime_type == "application/x-gzip" and m_request.support_gzip() )
//		response.set_header("content-encoding", "gzip");

	auto file_size = fs::file_size(m_sio.url_name);

	m_sio.response
			.set_header("content-length", file_size)
			.set_header("content-type"  , mime_type)
			.write();

	if( file_size == 0 )
		return ;

	auto buf_size = tcp_ip_buffer_size();
	char *fr_buf = new char[buf_size] {0};

	for(;;)
	{
		std::size_t size = m_file.readsome(fr_buf, buf_size);
		if( size == 0 )
			break;

		m_sio.response.write_body(size, fr_buf);
		if( size < buf_size )
			break;
	}
	delete[] fr_buf;
}

void static_resource_service::range_transfer(const std::string &range_str)
{
	using namespace std::chrono;

	// bytes=x-y, m-n, i-j ...
	if( range_str.substr(0,6) != "bytes=" )
		return m_sio.return_to_null(http::hs_range_not_satisfiable);

	// x-y, m-n, i-j ...
	auto range_list_str = range_str.substr(6);
	if( range_list_str.empty() )
		return m_sio.return_to_null(http::hs_range_not_satisfiable);

	// (x-y) ( m-n) ( i-j) ...
	auto range_list = string_split(range_list_str, ",");
	auto mime_type = get_mime_type(m_sio.url_name);

	m_sio.response.set_status(http::hs_partial_content);
	std::list<range_value> range_value_queue;

	if( range_list.size() == 1 )
	{
		range_value_queue.emplace_back();
		auto &range_value = range_value_queue.back();

		if( range_parsing(trimmed(range_list[0]), range_value.begin, range_value.end, range_value.size) == false )
			return m_sio.return_to_null(http::hs_range_not_satisfiable);

		m_sio.response
				.set_header("accept-ranges" , "bytes")
				.set_header("content-type"  , mime_type)
				.set_header("content-length", range_value.size)
				.set_header("content-range" , "{}-{}/{}", range_value.begin, range_value.end, range_value.size);

		return send_range("", "", range_value_queue);
	} // if( rangeList.size() == 1 )

	auto boundary = fmt::format("{}_{}", __func__, duration_cast<milliseconds>
								(system_clock::now().time_since_epoch()).count());

	m_sio.response.set_header("content-type", "multipart/byteranges; boundary=" + boundary);

	auto ct_line = "content-type: " + mime_type;
	std::size_t content_length = 0;

	for(auto &str : range_list)
	{
		range_value_queue.emplace_back();
		auto &range_value = range_value_queue.back();

		if( range_parsing(trimmed(str), range_value.begin, range_value.end, range_value.size) == false )
			return m_sio.return_to_null(http::hs_range_not_satisfiable);

		range_value.cr_line = fmt::format("content-range: bytes {}-{}/{}",
										  range_value.begin, range_value.end, fs::file_size(m_sio.url_name));
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
		content_length += 2 + boundary.size() + 2 +       // --boundary<CR><LF>
						 ct_line.size() + 2 +             // Content-Type: xxx<CR><LF>
						 range_value.cr_line.size() + 2 + // Content-Range: bytes 3-11/96<CR><LF>
						 2 +                              // <CR><LF>
						 range_value.size + 2;            // 012345678<CR><LF>
	}

	content_length += 2 + boundary.size() + 2 + 2;     // --boundary--<CR><LF>
	m_sio.response
			.set_header("content-length", content_length)
			.set_header("accept-ranges" , "bytes");
	send_range(boundary, ct_line, range_value_queue);
}

bool static_resource_service::range_parsing
(const std::string &range_str, std::size_t &begin, std::size_t &end, std::size_t &size)
{
	auto list = string_split(range_str, '-', false);

	if( list.size() > 2 )
	{
		log_debug("Range format error: {}.", range_str);
		return false;
	}

	auto file_size = fs::file_size(m_sio.url_name);

	if( list[0].empty() )
	{
		if( list[1].empty() )
		{
			log_debug("Range format error.");
			return false;
		}

		std::size_t size = 0;
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

void static_resource_service::send_range
(const std::string &boundary, const std::string &ct_line, std::list<range_value> &range_value_queue)
{
	assert(not range_value_queue.empty());
	m_sio.response.write();

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

				m_sio.response.write_body(s, buf);
				delete[] buf;
				flag = false;
			}
			else
			{
				auto buf = new char[buf_size] {0};
				auto s = m_file.readsome(buf, buf_size);

				m_sio.response.write_body(s, buf);
				value.size -= buf_size;
				delete[] buf;
			}
		}
		while(flag);
		return ;
	}

	for(auto &value : range_value_queue)
	{
		m_sio.response.write_body("--" + boundary + "\r\n" +
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

				m_sio.response.write_body(s + 2, buf);
				delete[] buf;
				flag = false;
			}
			else
			{
				auto buf = new char[buf_size] {0};
				auto s = m_file.readsome(buf, buf_size);

				m_sio.response.write_body(s, buf);
				value.size -= buf_size;
				delete[] buf;
			}
		}
		while(flag);
	}
	m_sio.response.write_body("--" + boundary + "--\r\n");
}

std::size_t static_resource_service::tcp_ip_buffer_size() const
{
	tcp::socket::send_buffer_size attr;
	m_sio.response.socket().get_option(attr);
	return attr.value();
}

}} //namespace gts::web
