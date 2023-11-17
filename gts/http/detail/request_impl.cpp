
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include "request_impl.h"

using namespace std::chrono;

namespace gts { namespace http
{

tcp_socket &request_impl::socket() const
{
	if( m_socket == nullptr )
		gts_log_fatal("gts::http::request: socket has been taken away. (gts::http::response::take)");
	return *m_socket;
}

std::size_t request_impl::tcp_ip_buffer_size() const
{
	tcp::socket::send_buffer_size attr;
	socket().get_option(attr);
	return attr.value();
}

void request_impl::finish(std::string &body)
{
	auto it = m_headers.find(header::connection);
	if( it == m_headers.end() )
		m_keep_alive = m_version != "1.0";
	else
		m_keep_alive = str_to_lower(it->second) != "close";

	it = m_headers.find(header::accept_encoding);
	if( it == m_headers.end() )
		m_support_gzip = false;
	else
	{
		for(auto &str : str_split(it->second, ","))
		{
			if( str_to_lower(str_trimmed(str)) == "gzip" )
			{
				m_support_gzip = true;
				break;
			}
		}
	}
	m_body = std::move(body);
	it = m_headers.find(header::content_length);

	if( it != m_headers.end() )
	{
		auto tplen = it->second.get<std::size_t>();
		if( m_body.size() > tplen )
			m_body = m_body.substr(0, tplen);

		m_rb_status = tplen > 0 ? rb_status::length : rb_status::finished;
		m_rb_context = tplen;
	}
	else if( m_version == "1.1" )
	{
		it = m_headers.find(header::transfer_encoding);
		if( it == m_headers.end() or it->second != "chunked" )
			m_rb_status = rb_status::finished;
		else
		{
			m_rb_status = rb_status::wait_size;
			m_rb_context = m_body;
			m_body.clear();
		}
	}
	else
	{
		m_rb_status = rb_status::finished;
		m_rb_context = std::size_t(0);
	}
	if( m_version == "1.1" )
	{
		auto it = m_headers.find("connection");
		if( it != m_headers.end() and str_to_lower(it->second) == "upgrade" )
		{
			it = m_headers.find("upgrade");
			if( it != m_headers.end() and str_to_lower(it->second) == "websocket" )
				m_websocket = true;
		}
	}
}

std::size_t request_impl::read_body_length_mode(std::error_code &error, void *buf, std::size_t size)
{
	auto &tplen = m_rb_context.get_value<std::size_t>();
	if( size > tplen )
		size = tplen;

	std::size_t sum = 0;
	if( size > m_body.size() )
	{
		if( not m_body.empty() )
		{
			memcpy(buf, m_body.c_str(), m_body.size());
			tplen -= m_body.size();
			size -= m_body.size();

			sum += m_body.size();
			m_body.clear();
		}
		auto &sock = socket();
		char *cbuf = reinterpret_cast<char*>(buf) + sum;

		while( tplen > 0 and size > 0 )
		{
			auto res = sock.read_some(cbuf, size, seconds(30), error);
			cbuf += res;
			sum += res;

			size -= res;
			tplen -= res;

            if( not error )
                continue;
            else if( res > 0 )
                error = std::error_code();
            else
            {
                if( sum > 0 )
                    error = std::error_code();
                break;
            }
		}
	}
	else
	{
		memcpy(buf, m_body.c_str(), size);
		m_body.erase(0,size);
		tplen -= size;
		sum = size;
	}
	if( tplen == 0 )
    {
		m_rb_status = rb_status::finished;
        error = std::error_code();
    }
	return sum;
}

std::size_t request_impl::read_body_chunked_mode(std::error_code &error, void *buf, std::size_t size)
{
	if( size <= m_body.size() )
	{
		memcpy(buf, m_body.c_str(), size);
		m_body.erase(0,size);
		return size;
	}
	std::size_t sum = 0;
	memcpy(buf, m_body.c_str(), m_body.size());

	size -= m_body.size();
	sum += m_body.size();
	m_body.clear();

	if( m_rb_status == rb_status::chunked_end )
	{
		m_rb_status = rb_status::finished;
		return sum;
	}
	auto &abuf = m_rb_context.get_value<std::string>();
	if( abuf.empty() )
	{
		auto tcp_size = tcp_ip_buffer_size();
		char *tmp_buf = new char[tcp_size] {0};

		auto res = socket().read_some(tmp_buf, tcp_size, seconds(30), error);
		abuf.append(tmp_buf, res);

		delete[] tmp_buf;
		if( error )
        {
            if( res > 0 )
                error = std::error_code();
            else
            {
                if( sum > 0 )
                    error = std::error_code();
                return sum;
            }
        }
	}
	std::size_t _size = 0;
	auto lambda_reset = [&](const char *msg)
	{
		m_rb_status = rb_status::finished;
		error = std::make_error_code(std::errc::wrong_protocol_type);
		gts_log_warning(msg);
	};
	while( not abuf.empty() )
	{
		auto pos = abuf.find("\r\n");
		if( pos == abuf.npos )
		{
			if( abuf.size() >= 1024 )
			{
				lambda_reset("Http protocol format error.");
				return 0;
			}
			break;
		}
		auto line_buf = abuf.substr(0, pos + 2);
		abuf.erase(0, pos + 2);

		if( m_rb_status == rb_status::wait_size )
		{
			line_buf.erase(pos);
			auto pos = line_buf.find(";");
			if( pos != line_buf.npos )
				line_buf.erase(pos);

			if( line_buf.size() > 16 )
			{
				lambda_reset("Http protocol format error.");
				return 0;
			}
			try {
				_size = static_cast<std::size_t>(std::stoull(line_buf, nullptr, 16));
			} catch(...) {
				lambda_reset("Http protocol format error.");
				return 0;
			}
			m_rb_status = _size == 0 ? rb_status::wait_headers : rb_status::wait_content;
		}
		else if( m_rb_status == rb_status::wait_content )
		{
			line_buf.erase(pos);
			if( _size < line_buf.size() )
				_size = line_buf.size();
			else
				m_rb_status = rb_status::wait_size;
			m_body += line_buf;
		}
		else if( m_rb_status == rb_status::wait_headers )
		{
			if( line_buf == "\r\n" )
			{
				m_rb_status = rb_status::chunked_end;
				break;
			}
			auto colon_index = line_buf.find(':');
			if( colon_index == line_buf.npos )
			{
				lambda_reset("Invalid header line.");
				return 0;
			}
			auto header_key = str_to_lower(str_trimmed(line_buf.substr(0, colon_index)));
			auto header_value = from_percent_encoding(str_trimmed(line_buf.substr(colon_index + 1)));

			if( header_key != "cookie" )
			{
				m_headers[header_key] = header_value;
				continue;
			}
			auto list = str_split(header_value, ";");
			for(auto &statement : list)
			{
				statement = str_trimmed(statement);
				auto pos = statement.find("=");

				if( pos == statement.npos )
				{
					lambda_reset("Invalid header line.");
					return 0;
				}
				header_key = str_trimmed(statement.substr(0,pos));
				header_value = str_trimmed(statement.substr(pos+1));
				m_cookies[header_key] = header_value;
			}
		}
	}
	if( size <= m_body.size() )
	{
		memcpy(buf, m_body.c_str(), size);
		m_body.erase(0,size);
		return sum + size;
	}
	memcpy(buf, m_body.c_str(), m_body.size());
	sum += m_body.size();
	m_body.clear();

	if( m_rb_status == rb_status::chunked_end )
		m_rb_status = rb_status::finished;
	return sum;
}

}} //namespace gts::http
