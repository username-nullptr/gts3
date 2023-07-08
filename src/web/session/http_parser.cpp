#include "http_parser.h"
#include "request_impl.h"

#include "gts/http/request.h"
#include "gts/algorithm.h"
#include "gts/log.h"

using namespace gts::http;

namespace gts { namespace web
{

http_parser::http_parser(int buf_size) :
	m_cache(new request())
{
	m_buffer.reserve(buf_size);
}

http_parser::~http_parser()
{
	if( m_cache )
		delete m_cache;
}

std::shared_ptr<request> http_parser::write(const std::string &data)
{
	m_buffer.append(data);
	for(;;)
	{
		auto pos = m_buffer.find("\r\n");
		if( pos == std::string::npos )
		{
			if( m_buffer.size() < 1024 )
				return {};

			else if( m_state == waiting_request )
			{
				log_info("Request line too long.");
				m_buffer.clear();
			}

			else if( m_state == reading_headers )
			{
				log_info("Header line too long.");
				reset();
			}
			return std::make_shared<request>();
		}

		auto line_buf = m_buffer.substr(0, pos + 2);
		m_buffer.erase(0, pos + 2);

		if( m_state == waiting_request )
			state_handler_waiting_request(line_buf);

		else if( m_state == reading_headers )
		{
			auto _request = state_handler_reading_headers(line_buf);
			if( _request )
				return std::shared_ptr<request>(_request);
		}
	}
	return {};
}

bool http_parser::state_handler_waiting_request(const std::string &line_buf)
{
	auto request_line_parts = string_split(line_buf, ' ');
	if( request_line_parts.size() != 3 or not starts_with(to_upper(request_line_parts[2]), "HTTP/") )
	{
		log_info("Invalid request line.");
		m_buffer.clear();
		return false;
	}

	auto method = from_method_string(request_line_parts[0]);
	if( method == METHOD_UNKNOWN )
	{
		log_info("Invalid http method.");
		m_buffer.clear();
		return false;
	}

	if( m_cache == nullptr )
		m_cache = new request();

	m_cache->m_impl->m_method  = method;
	m_cache->m_impl->m_version = request_line_parts[2].substr(5,3);

	auto resource_line = from_percent_encoding(request_line_parts[1]);
	auto pos = resource_line.find("?");

	if( pos == std::string::npos )
		m_cache->m_impl->m_path = resource_line;

	else
	{
		m_cache->m_impl->m_path = resource_line.substr(0, pos);
		m_cache->m_impl->m_parameters_string = resource_line.substr(pos + 1);

		for(auto &para_str : string_split(m_cache->m_impl->m_parameters_string, "&"))
		{
			auto pos = para_str.find("=");
			if( pos == std::string::npos )
				m_cache->m_impl->m_parameters.emplace(para_str, "");
			else
				m_cache->m_impl->m_parameters.emplace(para_str.substr(0, pos), para_str.substr(pos+1));
		}
	}

	auto n_it = std::unique(m_cache->m_impl->m_path.begin(), m_cache->m_impl->m_path.end(), [](char c0, char c1){
		return c0 == c1 and c0 == '/';
	});
	if( n_it != m_cache->m_impl->m_path.end() )
		m_cache->m_impl->m_path.erase(n_it, m_cache->m_impl->m_path.end());

	m_state = reading_headers;
	return true;
}

request *http_parser::state_handler_reading_headers(const std::string &line_buf)
{
	if( line_buf == "\r\n" )
		return next_request_ready();

	int colon_index = line_buf.find(':');

	if( colon_index < 0 )
	{
		log_info("Invalid header line.");
		reset();
		return new request();
	}

	auto header_key = to_lower(trimmed(line_buf.substr(0, colon_index)));
	auto header_value = from_percent_encoding(trimmed(line_buf.substr(colon_index + 1)));

	m_cache->m_impl->m_headers[header_key] = header_value;
	return nullptr;
}

request *http_parser::next_request_ready()
{
	m_state = waiting_request;

	if( not m_buffer.empty() )
	{
		auto it = m_cache->m_impl->m_headers.find("content-length");
		if( it != m_cache->m_impl->m_headers.end() and not it->second.empty() )
		{
			std::size_t size = 0;
			std::sscanf(it->second.c_str(), "%zu", &size);
			m_cache->m_impl->m_body = m_buffer.substr(0, size);
		}
		m_buffer.clear();
	}

	auto request = m_cache;
	m_cache = nullptr;

	request->m_impl->finish();
	return request;
}

void http_parser::reset()
{
	m_state = waiting_request;
	m_buffer.clear();

	delete m_cache;
	m_cache = nullptr;
}

}} //namespace gts::web
