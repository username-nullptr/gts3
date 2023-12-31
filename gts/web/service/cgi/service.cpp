
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

#include "service.h"
#include "gts/web/global.h"
#include "gts/http/detail/request_impl.h"

#include "gts/web/service/service_io.h"
#include "gts/web/config_key.h"
#include "gts/web/types.h"
#include "gts/settings.h"

#include "gts/private/app_info.h"
#include <cppfilesystem>
#include <iostream>

#ifdef GTS_ENABLE_SSL
# include "gts/gts_config_key.h"
#endif //ssl

GTS_WEB_NAMESPACE_BEGIN

cgi_service::cgi_service(service_io &sio) :
	m_sio(sio), m_cgi(io_context(), sio.url_name)
{

}

cgi_service::~cgi_service()
{
	if( m_sock_read_buf )
		delete m_sock_read_buf;
}

static environments g_cgi_env;

void cgi_service::init()
{
	auto &_settings = settings::global_instance();
	auto env_list = str_split(_settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_ENV), ";");

	for(auto &env : env_list)
	{
		auto pos = env.find("=");

		if( pos == std::string::npos )
			g_cgi_env.emplace(str_trimmed(env), "");
		else
			g_cgi_env.emplace(str_trimmed(env.substr(0, pos)), str_trimmed(env.substr(pos + 1)));
	}

#ifdef GTS_ENABLE_SSL
	auto crt_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_CRT_FILE, GTS_SSL_CRT_DEFAULT_FILE);
	if( not crt_file.empty() )
	{
		crt_file = app::absolute_path(crt_file);
		g_cgi_env.emplace("SSL_CRT_FILE", crt_file);
	}

	auto key_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_KEY_FILE, GTS_SSL_KEY_DEFAULT_FILE);
	if( not key_file.empty() )
	{
		key_file = app::absolute_path(key_file);
		g_cgi_env.emplace("SSL_KEY_FILE", key_file);
	}
#endif //ssl
}

std::string cgi_service::exists() const
{
	if( not fs::exists(m_sio.url_name) )
	{
#ifdef _WINDOWS
		file_name = m_sio.url_name + ".exe";
		if( not fs::exists(file_name) )
#endif //windows
			return "";
	}
	return m_sio.url_name;
}

bool cgi_service::call(std::string file_name)
{
	if( file_name.empty() )
	{
		file_name = exists();
		if( file_name.empty() )
			return false;
	}
	for(auto &pair : g_cgi_env)
		m_cgi.add_env(pair.first, pair.second);

	auto parameter = m_sio.request().parameters_string();
	if( parameter.empty() )
		parameter = "/";

	auto file_path = gts::file_path(file_name);
	m_cgi.set_work_path(file_path)
		 .add_env("REQUEST_METHOD"   , m_sio.request().method()                            )
		 .add_env("QUERY_STRING"     , parameter                                           )
		 .add_env("SCRIPT_NAME"      , m_sio.url_name                                      )
		 .add_env("SCRIPT_FILENAME"  , file_path                                           )
		 .add_env("REMOTE_ADDR"      , m_sio.socket.remote_endpoint().address().to_string())
		 .add_env("GATEWAY_INTERFACE", "CGI/1.1"                                           )
		 .add_env("SERVER_NAME"      , m_sio.socket.local_endpoint().address().to_string() )
		 .add_env("SERVER_PORT"      , m_sio.socket.local_endpoint().port()                )
		 .add_env("SERVER_PROTOCOL"  , "HTTP/" + m_sio.request().version()                 )
		 .add_env("DOCUMENT_ROOT"    , resource_root()                                     )
		 .add_env("SERVER_SOFTWARE"  , "GTS/1.0(GTS/" GTS_VERSION_STR ")"                  );

	for(auto &pair : m_sio.request().headers())
		m_cgi.add_env("HTTP_" + str_to_upper(replace_http_to_env(pair.first)), pair.second);

	for(auto &pair : m_sio.request().cookies())
		m_cgi.add_env("COOKIE_" + str_to_upper(replace_http_to_env(pair.first)), pair.second);

	auto it = m_sio.request().headers().find("content-length");
	if( it != m_sio.request().headers().end() )
		m_content_length = it->second.get<std::size_t>();

	if( m_cgi.start() == false )
	{
		m_sio.return_to_null(http::hs_forbidden);
		return true;
	}
	async_read_cgi();

	tcp::socket::send_buffer_size attr;
	m_sio.socket.get_option(attr);
	m_tcp_buf_size = attr.value();
	m_sock_read_buf = new char[m_tcp_buf_size] {0};

	auto res = m_sio.request().read_body(m_sock_read_buf, m_tcp_buf_size);
	if( res > 0 )
	{
		m_content_length -= res;
		async_write_cgi(m_sock_read_buf, res);
	}
	else
		async_read_socket();

	int pro_res = 0;
	bool pro_is_nor = m_cgi.join(&pro_res);

	if( m_counter > 0 )
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_condition.wait(locker);
	}
	gts_log_debug("cgi '{}' finished.", m_cgi.file());

	if( not m_sio.response().is_writed() and (not pro_is_nor or pro_res != 0) )
		m_sio.return_to_null(http::hs_internal_server_error);
	return true;
}

void cgi_service::async_write_socket(const char *buf, std::size_t buf_size)
{
	++m_counter;
	m_sio.socket.async_write_some(buf, buf_size, [this, buf, buf_size](const asio::error_code &error, std::size_t size)
	{
		--m_counter;
		if( error )
		{
			m_cgi.terminate();
			if( m_counter == 0 )
				m_condition.notify_one();
			return ;
		}

		if( size < buf_size )
			async_write_socket(buf + size, buf_size - size);
		else
			async_read_cgi();
	});
}

void cgi_service::async_read_socket()
{
	auto buf_size = m_content_length;
	if( buf_size > m_tcp_buf_size )
		buf_size = m_tcp_buf_size;

	else if( buf_size == 0 )
		return ;

	++m_counter;
	m_sio.socket.async_read_some(m_sock_read_buf, buf_size, [this](const asio::error_code &error, std::size_t size)
	{
		--m_counter;
		if( error or size == 0 or not m_cgi.is_running() )
		{
			if( m_counter == 0 )
				m_condition.notify_one();
			return ;
		}

		m_content_length -= size;
		async_write_cgi(m_sock_read_buf, size);
	});
}

void cgi_service::async_write_cgi(const char *buf, std::size_t buf_size)
{
	++m_counter;
	m_cgi.async_write_some(buf, buf_size, [this, buf, buf_size](const asio::error_code &error, std::size_t size)
	{
		--m_counter;
		if( error )
		{
			if( m_counter == 0 )
				m_condition.notify_one();
			return ;
		}

		else if( size < buf_size )
			async_write_cgi(buf + size, buf_size - size);

		else if( m_content_length > 0 )
			async_read_socket();

		else if( m_counter == 0 )
			m_condition.notify_one();
	});
}

void cgi_service::async_read_cgi()
{
	++m_counter;
	m_cgi.async_read_some(m_cgi_read_buf, BUF_SIZE,
						  [this](const asio::error_code&, std::size_t size)
	{
		--m_counter;
		if( size > 0 )
			async_write_socket(m_cgi_read_buf, size);
		else if( m_counter == 0 )
			m_condition.notify_one();
	});
}

std::string cgi_service::replace_http_to_env(const std::string &str)
{
	auto result = str;
	for(auto &c : result)
	{
		if( c == '-' )
			c = '_';
	}
	return result;
}

GTS_WEB_NAMESPACE_END
