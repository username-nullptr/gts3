#ifndef CGI_SERVICE_H
#define CGI_SERVICE_H

#include "service/service_io.h"
#include "gts/web_config_key.h"
#include "gts/algorithm.h"
#include "app_info.h"
#include "settings.h"
#include "gts_log.h"
#include "process.h"
#include "global.h"

#include <condition_variable>
#include <cppfilesystem>
#include <iostream>
#include <cstdio>
#include <atomic>

namespace gts { namespace web
{

#define _BUF_SIZE  65536

template <class asio_socket>
class GTS_DECL_HIDDEN cgi_service
{
	DISABLE_COPY(cgi_service)
	typedef service_io<asio_socket>  s_io;

public:
	explicit cgi_service(s_io &sio);
	void call();

private:
	void async_write_socket(const char *buf, std::size_t buf_size);
	void async_read_socket();

private:
	void async_write_cgi(const char *buf, std::size_t buf_size);
	void async_read_cgi();

private:
	std::string replace_http_to_env(const std::string &str);

private:
	s_io &m_sio;
	process m_cgi;

private:
	char m_sock_read_buf[_BUF_SIZE];
	char m_cgi_read_buf[_BUF_SIZE];
	std::size_t m_content_length = 0;

private:
	std::atomic_int m_counter {0};
	std::condition_variable m_condition;
	std::mutex m_mutex;
};

class GTS_DECL_HIDDEN cgi_service_config
{
public:
	static std::map<std::string, std::string> cgi_env;
	static void init();
};

template<class asio_socket>
cgi_service<asio_socket>::cgi_service(s_io &sio) :
	m_sio(sio), m_cgi(io_context(), sio.url_name)
{

}

template <class asio_socket>
void cgi_service<asio_socket>::call()
{
	if( not fs::exists(m_sio.url_name) )
	{
#ifdef _WINDOWS
		file_name += ".exe";
		if( not fs::exists(file_name) )
#endif //windows
			return m_sio.return_to_null(http::hs_not_found);
	}

	auto parameter = m_sio.request.parameters_string();
	if( parameter.empty() )
		parameter = "/";

	auto file_path = gts::file_path(m_sio.url_name);

	m_cgi.add_env("REQUEST_METHOD"   , m_sio.request.method());
	m_cgi.add_env("QUERY_STRING"     , parameter);
	m_cgi.add_env("SCRIPT_NAME"      , m_sio.url_name);
	m_cgi.add_env("SCRIPT_FILENAME"  , file_path);
	m_cgi.add_env("REMOTE_ADDR"      , m_sio.socket.remote_endpoint().address().to_string());
	m_cgi.add_env("GATEWAY_INTERFACE", "CGI/1.1");
	m_cgi.add_env("SERVER_NAME"      , m_sio.socket.local_endpoint().address().to_string());
	m_cgi.add_env("SERVER_PORT"      , m_sio.socket.local_endpoint().port());
	m_cgi.add_env("SERVER_PROTOCOL"  , "HTTP/" + m_sio.request.version());
	m_cgi.add_env("DOCUMENT_ROOT"    , service_io_config::resource_path);
	m_cgi.add_env("SERVER_SOFTWARE"  , "GTS/1.0(GTS/" GTS_VERSION_STR ")");

	for(auto &pair : m_sio.request.headers())
		m_cgi.add_env("HTTP_" + to_upper(replace_http_to_env(pair.first)), pair.second);

	m_cgi.set_work_path(file_path);

	auto it = m_sio.request.headers().find("content-length");
	if( it != m_sio.request.headers().end() )
		std::sscanf(it->second.c_str(), "%zu", &m_content_length);

	m_cgi.start();
	async_read_cgi();

	m_sio.socket.non_blocking(true);

	if( m_sio.request.body().size() > 0 )
	{
		m_content_length -= m_sio.request.body().size();
		async_write_cgi(m_sio.request.body().c_str(), m_sio.request.body().size());
	}
	else
		async_read_socket();

	m_cgi.join();
	if( m_counter > 0 )
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_condition.wait(locker);
	}
	log_debug("cgi '{}' finished.", m_cgi.file());
}

template <class asio_socket>
void cgi_service<asio_socket>::async_write_socket(const char *buf, std::size_t buf_size)
{
	++m_counter;
	m_sio.socket.async_write_some
			(asio::buffer(buf, buf_size),
			 [this, buf, buf_size](const asio::error_code &error, std::size_t size)
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

template <class asio_socket>
void cgi_service<asio_socket>::async_read_socket()
{
	auto buf_size = m_content_length;

	if( buf_size > _BUF_SIZE )
		buf_size = _BUF_SIZE;

	else if( buf_size == 0 )
		return ;

	++m_counter;
	m_sio.socket.async_read_some
			(asio::buffer(m_sock_read_buf, buf_size),
			 [this](const asio::error_code &error, std::size_t size)
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

template <class asio_socket>
void cgi_service<asio_socket>::async_write_cgi(const char *buf, std::size_t buf_size)
{
	++m_counter;
	m_cgi.async_write(buf, buf_size, [this, buf, buf_size](const asio::error_code &error, std::size_t size)
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

template <class asio_socket>
void cgi_service<asio_socket>::async_read_cgi()
{
	++m_counter;
	m_cgi.async_read(m_cgi_read_buf, _BUF_SIZE,
					 [this](const asio::error_code&, std::size_t size)
	{
		--m_counter;
		if( size > 0 )
			async_write_socket(m_cgi_read_buf, size);
		else if( m_counter == 0 )
			m_condition.notify_one();
	});
}

template <class asio_socket>
std::string cgi_service<asio_socket>::replace_http_to_env(const std::string &str)
{
	auto result = str;
	for(auto &c : result)
	{
		if( c == '-' )
			c = '_';
	}
	return result;
}

}} //namespace gts::web


#endif //CGI_SERVICE_H
