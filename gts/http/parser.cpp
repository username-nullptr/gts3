#include "parser.h"
#include "detail/request_impl.h"

#include "gts/http/request.h"
#include "gts/algorithm.h"
#include "gts/log.h"

namespace gts { namespace http
{

enum class state
{
	waiting_request,
	reading_headers
};

class GTS_DECL_HIDDEN parser_impl
{
	GTS_DISABLE_COPY_MOVE(parser_impl)

public:
	parser_impl(int buf_size) :
		m_cache(new request())
	{
		m_buffer.reserve(buf_size);
	}
	~parser_impl()
	{
		if( m_cache )
			delete m_cache;
	}

public:
	bool state_handler_waiting_request(const std::string &line_buf)
	{
		auto request_line_parts = str_split(line_buf, ' ');
		if( request_line_parts.size() != 3 or not str_starts_with(str_to_upper(request_line_parts[2]), "HTTP/") )
		{
			gts_log_info("Invalid request line.");
			m_buffer.clear();
			return false;
		}

		auto method = from_method_string(request_line_parts[0]);
		if( method == METHOD_UNKNOWN )
		{
			gts_log_info("Invalid http method.");
			m_buffer.clear();
			return false;
		}

		if( m_cache == nullptr )
			m_cache = new request();

		m_cache->m_impl->m_method  = method;
		m_cache->m_impl->m_version = request_line_parts[2].substr(5,3);

		auto resource_line = from_percent_encoding(request_line_parts[1]);
		auto pos = resource_line.find("?");

		if( pos == resource_line.npos )
			m_cache->m_impl->m_path = resource_line;

		else
		{
			m_cache->m_impl->m_path = resource_line.substr(0, pos);
			m_cache->m_impl->m_parameters_string = resource_line.substr(pos + 1);

			for(auto &para_str : str_split(m_cache->m_impl->m_parameters_string, "&"))
			{
				auto pos = para_str.find("=");
				if( pos == para_str.npos )
					m_cache->m_impl->m_parameters.emplace(para_str, para_str);
				else
					m_cache->m_impl->m_parameters.emplace(para_str.substr(0, pos), para_str.substr(pos+1));
			}
		}

		auto n_it = std::unique(m_cache->m_impl->m_path.begin(), m_cache->m_impl->m_path.end(), [](char c0, char c1){
			return c0 == c1 and c0 == '/';
		});
		if( n_it != m_cache->m_impl->m_path.end() )
			m_cache->m_impl->m_path.erase(n_it, m_cache->m_impl->m_path.end());

		m_state = state::reading_headers;
		return true;
	}

	request *state_handler_reading_headers(const std::string &line_buf)
	{
		if( line_buf == "\r\n" )
			return next_request_ready();

		auto colon_index = line_buf.find(':');
		if( colon_index == line_buf.npos )
		{
			gts_log_info("Invalid header line.");
			reset();
			return new request();
		}

		auto key = str_to_lower(str_trimmed(line_buf.substr(0, colon_index)));
		auto value = from_percent_encoding(str_trimmed(line_buf.substr(colon_index + 1)));

		if( key != "cookie" )
		{
			m_cache->m_impl->m_headers[key] = value;
			return nullptr;
		}
		auto list = str_split(value, ";");
		for(auto &statement : list)
		{
			statement = str_trimmed(statement);
			auto pos = statement.find("=");

			if( pos == statement.npos )
			{
				gts_log_info("Invalid header line.");
				reset();
				return new request();
			}

			key = str_trimmed(statement.substr(0,pos));
			value = str_trimmed(statement.substr(pos+1));
			m_cache->m_impl->m_cookies[key] = value;
		}
		return nullptr;
	}

	request *next_request_ready()
	{
		m_state = state::waiting_request;
		auto request = m_cache;
		m_cache = nullptr;

		request->m_impl->finish(m_buffer);
		return request;
	}

	void reset()
	{
		m_state = state::waiting_request;
		m_buffer.clear();

		delete m_cache;
		m_cache = nullptr;
	}

public:
	state m_state = state::waiting_request;
	std::string m_buffer;

public:
	request *m_cache = nullptr;
	service_context_ptr m_context;
};

/*-------------------------------------------------------------------------------------------------------------------------*/

parser::parser(int buf_size) :
	m_impl(new parser_impl(buf_size))
{

}

parser::~parser()
{
	delete m_impl;
}

bool parser::write(const std::string &data)
{
	m_impl->m_buffer.append(data);
	while( not m_impl->m_buffer.empty() )
	{
		auto pos = m_impl->m_buffer.find("\r\n");
		if( pos == m_impl->m_buffer.npos )
		{
			if( m_impl->m_buffer.size() < 1024 )
				return false;
			else if( m_impl->m_state == state::waiting_request )
			{
				gts_log_info("Request line too long.");
				m_impl->m_buffer.clear();
			}
			else if( m_impl->m_state == state::reading_headers )
			{
				gts_log_info("Header line too long.");
				m_impl->reset();
			}
			return false;
		}
		auto line_buf = m_impl->m_buffer.substr(0, pos + 2);
		m_impl->m_buffer.erase(0, pos + 2);

		if( m_impl->m_state == state::waiting_request )
			m_impl->state_handler_waiting_request(line_buf);

		else if( m_impl->m_state == state::reading_headers )
		{
			auto _request = m_impl->state_handler_reading_headers(line_buf);
			if( _request )
			{
				m_impl->m_context = std::make_shared<service_context>(_request);
				return true;
			}
		}
	}
	return false;
}

service_context_ptr parser::get_request(tcp_socket_ptr socket)
{
	if( m_impl->m_context == nullptr )
		return {};

	auto tmp = m_impl->m_context;
	tmp->request().m_impl->m_socket = socket;

	m_impl->m_context.reset();
	return tmp;
}

}} //namespace gts::http
