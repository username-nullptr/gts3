#include "service.h"
#include "gts_algorithm.h"
#include "web_config_key.h"
#include "settings.h"
#include "process.h"
#include "log.h"

#include <condition_variable>
#include <cppfilesystem>
#include <iostream>
#include <cstdio>
#include <atomic>

#define _BUF_SIZE  65535

namespace gts { namespace web
{

static std::string g_resource_path = _GTS_WEB_DEFAULT_RC_PATH;

static std::map<std::string, std::string> g_cgi_env;

static std::string replace_http_to_env(const std::string &str);

class GTS_DECL_HIDDEN cgi_session
{
	DISABLE_COPY(cgi_session)

public:
	cgi_session(service &_service, process &cgi);
	~cgi_session();

public:
	void join();

private:
	void async_write_socket(const char *buf, std::size_t buf_size);
	void async_read_socket();

private:
	void async_write_cgi(const char *buf, std::size_t buf_size);
	void async_read_cgi();

private:
	service &m_service;
	process &m_cgi;

private:
	char m_sock_read_buf[_BUF_SIZE];
	char m_cgi_read_buf[_BUF_SIZE];
	std::size_t m_content_length = 0;

private:
	std::atomic_int m_counter {0};
	std::condition_variable m_condition;
	std::mutex m_mutex;
};

/*-------------------------------------------------------------------------------------------------------------------*/

void service::call_cgi(const std::string &file_name)
{
	if( not fs::exists(file_name) )
	{
#ifdef _WINDOWS
		file_name += ".exe";
		if( not fs::exists(file_name) )
#endif //windows
			return return_to_null(http::hs_not_found);
	}

	auto parameter = request.parameters_string();
	if( parameter.empty() )
		parameter = "/";

	auto file_path = gts::file_path(file_name);
	process cgi(io_context(), file_name);

	cgi.add_env("REQUEST_METHOD"   , request.method());
	cgi.add_env("QUERY_STRING"     , parameter);
	cgi.add_env("SCRIPT_NAME"      , file_name);
	cgi.add_env("SCRIPT_FILENAME"  , file_path);
	cgi.add_env("REMOTE_ADDR"      , socket.remote_endpoint().address().to_string());
	cgi.add_env("GATEWAY_INTERFACE", "CGI/1.1");
	cgi.add_env("SERVER_NAME"      , socket.local_endpoint().address().to_string());
	cgi.add_env("SERVER_PORT"      , socket.local_endpoint().port());
	cgi.add_env("SERVER_PROTOCOL"  , "HTTP/" + request.version());
	cgi.add_env("DOCUMENT_ROOT"    , g_resource_path);
	cgi.add_env("SERVER_SOFTWARE"  , "GTS/1.0(GTS/" GTS_VERSION_STR ")");

	for(auto &pair : request.headers())
		cgi.add_env("HTTP_" + to_upper(replace_http_to_env(pair.first)), pair.second);

	cgi.set_work_path(file_path);
	cgi_session(*this, cgi).join();
}

void cgi_service_init()
{
	auto &_settings = settings::global_instance();
	g_resource_path = _settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_RC_PATH, g_resource_path);

	if( not starts_with(g_resource_path, "/") )
		g_resource_path = appinfo::dir_path() + g_resource_path;

	if( not ends_with(g_resource_path, _PS) )
		g_resource_path += _PS;

	auto env_list = string_split(_settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_ENV), ";");
	if( env_list.empty() )
		return ;

	for(auto &env : env_list)
	{
		auto pos = env.find("=");

		if( pos == std::string::npos )
			g_cgi_env.emplace(trimmed(env), "");

		else
			g_cgi_env.emplace(to_upper(trimmed(env.substr(0, pos))),
							  to_upper(trimmed(env.substr(pos + 1))));
	}
}

/*-------------------------------------------------------------------------------------------------------------------*/

static inline std::string replace_http_to_env(const std::string &str)
{
	auto result = str;
	for(auto &c : result)
	{
		if( c == '-' )
			c = '_';
	}
	return result;
}

cgi_session::cgi_session(service &_service, process &cgi) :
	m_service(_service), m_cgi(cgi)
{
	auto it = m_service.request.headers().find("content-length");
	if( it != m_service.request.headers().end() )
		std::sscanf(it->second.c_str(), "%zu", &m_content_length);

	m_cgi.start();
	async_read_cgi();

	m_service.socket.non_blocking(true);

	if( m_service.request.body().size() > 0 )
	{
		log_error("test=== {} / {}", m_service.request.body().size(), m_content_length);
		m_content_length -= m_service.request.body().size();
		async_write_cgi(m_service.request.body().c_str(), m_service.request.body().size());
	}
	else
		async_read_socket();
}

cgi_session::~cgi_session()
{
	m_cgi.cancel();
	m_service.socket.cancel();
	m_service.socket.non_blocking(false);
}

void cgi_session::join()
{
	m_cgi.join();
	if( m_counter > 0 )
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_condition.wait(locker);
	}
	log_debug("cgi '{}' finished.", m_cgi.file());
}

void cgi_session::async_write_socket(const char *buf, std::size_t buf_size)
{
	m_service.socket.async_write_some
			(asio::buffer(buf, buf_size),
			 [this, buf, buf_size](const asio::error_code &error, std::size_t size)
	{
		if( error )
		{
			m_cgi.terminate();
			return ;
		}
		else if( not m_cgi.is_running() )
			return ;

		if( size < buf_size )
			async_write_socket(buf + size, buf_size - size);
		else
			async_read_cgi();
	});
}

void cgi_session::async_read_socket()
{
	auto buf_size = m_content_length;

	if( buf_size > _BUF_SIZE )
		buf_size = _BUF_SIZE;

	else if( buf_size == 0 )
		return ;

	m_service.socket.async_read_some
			(asio::buffer(m_sock_read_buf, buf_size),
			 [this](const asio::error_code &error, std::size_t size)
	{
		if( error or size == 0 or not m_cgi.is_running() )
			return ;

		m_content_length -= size;
		async_write_cgi(m_sock_read_buf, size);
	});
}

void cgi_session::async_write_cgi(const char *buf, std::size_t buf_size)
{
	++m_counter;
	m_cgi.async_write(buf, buf_size, [this, buf, buf_size](const asio::error_code &error, std::size_t size)
	{
		if( --m_counter == 0 )
			m_condition.notify_one();

		if( error )
			return ;

		else if( size < buf_size )
			async_write_cgi(buf + size, buf_size - size);

		else if( m_content_length > 0 )
			async_read_socket();
	});
}

void cgi_session::async_read_cgi()
{
	++m_counter;
	m_cgi.async_read(m_cgi_read_buf, _BUF_SIZE,
					 [this](const asio::error_code&, std::size_t size)
	{
		if( --m_counter == 0 )
			m_condition.notify_one();

		if( size > 0 )
			async_write_socket(m_cgi_read_buf, size);
	});
}

}} //namespace gts::web
