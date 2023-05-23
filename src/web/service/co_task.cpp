#include "co_task.h"
#include "gts/algorithm.h"
#include "gts/web_config_key.h"
#include "app_info.h"
#include "settings.h"
#include "gts_log.h"
#include "service.h"

#include <asio/thread_pool.hpp>
#include <cppfilesystem>
#include <thread>
#include <chrono>

using namespace std::chrono;

namespace gts { namespace web
{

co_task::co_task(std::shared_ptr<tcp::socket> socket, std::shared_ptr<http::request> request) :
	m_socket(socket),
	m_request(request)
{

}

co_task::~co_task()
{

}

static asio::thread_pool *g_pool = nullptr;

static std::string g_cgi_path = _GTS_WEB_DEFAULT_CGI_PATH;

#define READ_PATH_CONFIG(_key, _arg) \
({ \
	_arg = _settings.read<std::string>(SINI_GROUP_WEB, _key, _arg); \
	if( not starts_with(_arg, "/") ) \
		_arg = appinfo::dir_path() + _arg; \
	if( not ends_with(_arg, _PS) ) \
		_arg += _PS; \
	if( not fs::exists(_arg) ) \
		fs::create_directories(_arg); \
})

void co_task::init()
{
	auto &_settings = settings::global_instance();
	int max_thread_count = _settings.read<int>(SINI_GROUP_WEB, SINI_WEB_THREAD_POOL_TC, 100);

	if( max_thread_count < 1 )
	{
		log_warning("Config: max thread count setting error.");
		max_thread_count = 1;
	}

	log_debug("Web: max thread count: {}", max_thread_count);
	g_pool = new asio::thread_pool(max_thread_count);

	READ_PATH_CONFIG(SINI_WEB_CGI_PATH, g_cgi_path);
	service::init();
}

void co_task::exit()
{
	service::exit();
	if( g_pool == nullptr )
		return ;

	g_pool->join();
	delete g_pool;
	g_pool = nullptr;
}

void co_task::start()
{
	m_socket->non_blocking(false);

	asio::post(*g_pool, [this]
	{
		run();
		io_context().post([this]
		{
			if( m_call_back )
				m_call_back();
		});
	});
}

void co_task::async_wait_next(std::function<void()> call_back)
{
	assert(call_back != nullptr);
	m_call_back = std::move(call_back);
}

void co_task::cancel()
{
	m_call_back = nullptr;
}

template <class asio_socket> static void GET    (service_io<asio_socket> &sio);
template <class asio_socket> static void POST   (service_io<asio_socket> &sio);
template <class asio_socket> static void PUT    (service_io<asio_socket> &sio);
template <class asio_socket> static void HEAD   (service_io<asio_socket> &sio);
template <class asio_socket> static void DELETE (service_io<asio_socket> &sio);
template <class asio_socket> static void OPTIONS(service_io<asio_socket> &sio);
template <class asio_socket> static void CONNECT(service_io<asio_socket> &sio);
template <class asio_socket> static void TRACH  (service_io<asio_socket> &sio);

void co_task::run()
{
	auto method = m_request->method();
	log_debug() << "URL:" << m_request->path() << method;

	service_io<tcp::socket> sio(*m_socket, *m_request);

	if(      method == "GET"     ) GET    (sio);
	else if( method == "POST"    ) POST   (sio);
	else if( method == "HEAD"    ) HEAD   (sio);
	else if( method == "PUT"     ) PUT    (sio);
	else if( method == "DELETE"  ) DELETE (sio);
	else if( method == "OPTIONS" ) OPTIONS(sio);
	else if( method == "CONNECT" ) CONNECT(sio);
	else if( method == "TRACH"   ) TRACH  (sio);

	if( not m_request->keep_alive() )
	{
		m_socket->shutdown(tcp::socket::shutdown_both);
		m_socket->close();
	}
}

/*----------------------------------------------------------------------------------------------------*/

#define _PARSING(_sio) \
({ \
	sio.url_name = _sio.request.path(); \
	if( sio.url_name.empty() ) \
		sio.url_name = "/index.html"; \
	else { \
		auto pos = sio.url_name.find("/", 1); \
		if( pos != std::string::npos ) { \
			auto prefix = sio.url_name.substr(0,pos); \
			if( prefix == "/cgi-bin" ) { \
				sio.url_name = g_cgi_path + sio.url_name.substr(pos+1); \
				return service::call_cgi_service(sio); \
			} else if ( prefix == "/plugin-lib" ) { \
				sio.url_name.erase(0, pos+1); \
				return service::call_plugin_service(sio); \
			} \
		} \
	} \
	if( fs::is_directory(sio.url_name) ) \
		return _sio.return_to_null(http::hs_not_found); \
	sio.url_name; \
})

template <class asio_socket>
static void GET(service_io<asio_socket> &sio)
{
	sio.url_name = service_io_config::resource_path + _PARSING(sio);
	service::call_static_resource_service(sio);
}

template <class asio_socket>
static void POST(service_io<asio_socket> &sio)
{
	_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

template <class asio_socket>
static void PUT(service_io<asio_socket> &sio)
{
	_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

template <class asio_socket>
static void HEAD(service_io<asio_socket> &sio)
{
	sio.url_name = sio.request.path();
	if( sio.url_name == "/" )
		return sio.return_to_null();

	auto pos = sio.url_name.find("/", 1);

	if( pos != std::string::npos )
	{
		auto prefix = sio.url_name.substr(0,pos);
		if( prefix == "/cgi-bin" )
		{
			if( fs::exists(g_cgi_path + sio.url_name.substr(pos+1)) )
				return sio.return_to_null();
			return sio.return_to_null(http::hs_not_found);
		}
		else if ( prefix == "/plugin-lib" )
		{
			sio.url_name.erase(0, pos+1);
			return service::call_plugin_service(sio);
		}
	}

	else if( fs::exists(service_io_config::resource_path + sio.url_name) )
		sio.return_to_null(http::hs_not_found);
	else
		sio.return_to_null();
}

template <class asio_socket>
static void DELETE(service_io<asio_socket> &sio)
{
	_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

template <class asio_socket>
static void OPTIONS(service_io<asio_socket> &sio)
{
	sio.return_to_null(http::hs_service_unavailable);
}

template <class asio_socket>
static void CONNECT(service_io<asio_socket> &sio)
{
	sio.return_to_null(http::hs_service_unavailable);
}

template <class asio_socket>
static void TRACH(service_io<asio_socket> &sio)
{
	sio.return_to_null(http::hs_service_unavailable);
}

}} //namespace gts::web
