#include "co_task.h"
#include "gts_algorithm.h"
#include "web_config_key.h"
#include "settings.h"
#include "service.h"
#include "log.h"

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

static std::atomic<std::size_t> g_counter {0};
static std::atomic<std::size_t> g_counter_max {1100};

static std::string g_resource_path = _GTS_WEB_DEFAULT_RC_PATH;
static std::string g_cgi_path      = _GTS_WEB_DEFAULT_CGI_PATH;

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
	int max_task_count = _settings.read<int>(SINI_GROUP_WEB, SINI_WEB_MAX_TASK_COUNT, 100);
	int block_queue_size = _settings.read<int>(SINI_GROUP_WEB, SINI_WEB_BLOCK_QUEUE_SIZE, 1000);

	if( max_task_count < 2 )
	{
		log_warning("Config: Max task count setting error.");
		max_task_count = 2;
	}
	if( block_queue_size < 0 )
	{
		log_warning("Config: Block queue size setting error.");
		block_queue_size = 0;
	}

	log_debug("Web: max task count: {}", max_task_count);
	log_debug("Web: block queue size: {}", block_queue_size);
	g_counter_max = max_task_count + block_queue_size;

	g_pool = new asio::thread_pool(max_task_count);

	READ_PATH_CONFIG(SINI_WEB_RC_PATH,  g_resource_path);
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
	if( g_counter >= g_counter_max )
	{
		asio::error_code error;
		m_socket->write_some(asio::buffer(fmt::format("HTTP/{} 413 Payload Too Large\r\n"
													  "content-length: 0\r\n"
													  "\r\n",
													  m_request->version())), error);
		if( m_request->version() == "1.0" )
		{
			m_socket->shutdown(tcp::socket::shutdown_both);
			m_socket->close();
		}
		if( m_call_back )
			m_call_back();

		return delete_later(this);
	}

	g_counter++;
	m_socket->non_blocking(false);

	asio::post(*g_pool, [this]
	{
		run();
		asio::post(io_context(), [this]
		{
			g_counter--;
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

static void GET    (service &s);
static void POST   (service &s);
static void PUT    (service &s);
static void HEAD   (service &s);
static void DELETE (service &s);
static void OPTIONS(service &s);
static void CONNECT(service &s);
static void TRACH  (service &s);

void co_task::run()
{
	auto method = m_request->method();
	log_debug() << "URL:" << m_request->path() << method;

	service _service(*m_socket, *m_request);

	if(      method == "GET"     ) GET    (_service);
	else if( method == "POST"    ) POST   (_service);
	else if( method == "HEAD"    ) HEAD   (_service);
	else if( method == "PUT"     ) PUT    (_service);
	else if( method == "DELETE"  ) DELETE (_service);
	else if( method == "OPTIONS" ) OPTIONS(_service);
	else if( method == "CONNECT" ) CONNECT(_service);
	else if( method == "TRACH"   ) TRACH  (_service);

	if( not m_request->keep_alive() )
	{
		m_socket->shutdown(tcp::socket::shutdown_both);
		m_socket->close();
	}
}

/*----------------------------------------------------------------------------------------------------*/

#define _PARSING(_s) \
({ \
	auto url_name = _s.request.path(); \
	if( url_name.empty() ) \
		url_name = "/index.html"; \
	else { \
		auto pos = url_name.find("/", 1); \
		if( pos != std::string::npos ) { \
			auto prefix = url_name.substr(0,pos); \
			if( prefix == "/cgi-bin" ) { \
				_s.call_cgi(g_cgi_path + url_name.substr(pos+1)); \
				return ; \
			} else if ( prefix == "/plugin-lib" ) { \
				_s.call_plugins(url_name.substr(pos+1)); \
				return ; \
			} \
		} \
	} \
	if( fs::is_directory(url_name) ) \
		return _s.return_to_null(http::hs_not_found); \
	url_name; \
})

static void GET(service &s)
{
	auto url_name = _PARSING(s);
	s.call_static_resource(g_resource_path + url_name);
}

static void POST(service &s)
{
	_PARSING(s);
	s.return_to_null(http::hs_not_implemented);
}

static void PUT(service &s)
{
	_PARSING(s);
	s.return_to_null(http::hs_not_implemented);
}

static void HEAD(service &s)
{
	std::string url_name = s.request.path();
	if( url_name == "/" )
		return s.return_to_null();

	auto pos = url_name.find("/", 1);

	if( pos != std::string::npos )
	{
		auto prefix = url_name.substr(0,pos);
		if( prefix == "/cgi-bin" )
		{
			if( fs::exists(g_cgi_path + url_name.substr(pos+1)) )
				return s.return_to_null();
			return s.return_to_null(http::hs_not_found);
		}
		else if ( prefix == "/plugin-lib" )
		{
			s.call_plugins(url_name.substr(pos+1));
			return ;
		}
	}

	else if( fs::exists(g_resource_path + url_name) )
		s.return_to_null(http::hs_not_found);
	else
		s.return_to_null();
}

static void DELETE(service &s)
{
	_PARSING(s);
	s.return_to_null(http::hs_not_implemented);
}

static void OPTIONS(service &s)
{
	s.return_to_null(http::hs_service_unavailable);
}

static void CONNECT(service &s)
{
	s.return_to_null(http::hs_service_unavailable);
}

static void TRACH(service &s)
{
	s.return_to_null(http::hs_service_unavailable);
}

}} //namespace gts::web
