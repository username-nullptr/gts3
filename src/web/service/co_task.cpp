#include "co_task.h"
#include "gts_algorithm.h"
#include "web_config_key.h"
#include "settings.h"
#include "log.h"

#include "http/response.h"
#include "http/request.h"
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

	plugin_service_init();
	cgi_service_init();
}

void co_task::exit()
{
	plugin_service_exit();
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

static void GET    (http::request &request, http::response &response, tcp::socket &socket);
static void POST   (http::request &request, http::response &response, tcp::socket &socket);
static void PUT    (http::request &request, http::response &response, tcp::socket &socket);
static void HEAD   (http::request &request, http::response &response, tcp::socket &socket);
static void DELETE (http::request &request, http::response &response, tcp::socket &socket);
static void OPTIONS(http::request &request, http::response &response, tcp::socket &socket);
static void CONNECT(http::request &request, http::response &response, tcp::socket &socket);
static void TRACH  (http::request &request, http::response &response, tcp::socket &socket);

void co_task::run()
{
	auto method = m_request->method();
	log_debug() << "URL:" << m_request->path() << method;

	http::response response(m_request->version());
	response.set_header("server", "gts3");

	if(      method == "GET"     ) GET    (*m_request, response, *m_socket);
	else if( method == "POST"    ) POST   (*m_request, response, *m_socket);
	else if( method == "HEAD"    ) HEAD   (*m_request, response, *m_socket);
	else if( method == "PUT"     ) PUT    (*m_request, response, *m_socket);
	else if( method == "DELETE"  ) DELETE (*m_request, response, *m_socket);
	else if( method == "OPTIONS" ) OPTIONS(*m_request, response, *m_socket);
	else if( method == "CONNECT" ) CONNECT(*m_request, response, *m_socket);
	else if( method == "TRACH"   ) TRACH  (*m_request, response, *m_socket);

	if( not m_request->keep_alive() )
	{
		m_socket->shutdown(tcp::socket::shutdown_both);
		m_socket->close();
	}
}

/*----------------------------------------------------------------------------------------------------*/

#define _PARSING(request) \
({ \
	auto url_name = request.path(); \
	if( url_name.empty() ) \
		url_name = "/index.html"; \
	else { \
		auto pos = url_name.find("/", 1); \
		if( pos != std::string::npos ) { \
			auto prefix = url_name.substr(0,pos); \
			if( prefix == "/cgi-bin" ) { \
				cgi_service(socket, request, response, g_cgi_path + url_name.substr(pos+1)); \
				return ; \
			} else if ( prefix == "/plugin-lib" ) { \
				plugins_service(socket, request, response, url_name.substr(pos+1)); \
				return ; \
			} \
		} \
	} \
	if( fs::is_directory(url_name) ) \
		return SEND_RESPONSE(http::hs_not_found); \
	url_name; \
})

#define SEND_RESPONSE(_status)  SEND_NULL_RESPONSE(socket, request, response, _status)

static void GET(http::request &request, http::response &response, tcp::socket &socket)
{
	auto url_name = _PARSING(request);
	static_resource_service(socket, request, response, g_resource_path + url_name);
}

static void POST(http::request &request, http::response &response, tcp::socket &socket)
{
	_PARSING(request);
	SEND_RESPONSE(http::hs_not_implemented);
}

static void PUT(http::request &request, http::response &response, tcp::socket &socket)
{
	_PARSING(request);
	SEND_RESPONSE(http::hs_not_implemented);
}

#define HEAD_SEND_OK() \
({ \
	response.set_status(http::hs_ok); \
	response.set_header("accept-ranges", "bytes"); \
	response.set_header("connection", "close"); \
	socket.write_some(asio::buffer(response.to_string())); \
	socket.shutdown(tcp::socket::shutdown_both); \
	socket.close(); \
})

static void HEAD(http::request &request, http::response &response, tcp::socket &socket)
{
	std::string url_name = request.path();
	if( url_name == "/" )
		return HEAD_SEND_OK();

	auto list = string_split(url_name, "/");

	if( list[0] == "plugin-lib" )
	{
		list.pop_front();

		if( fs::exists(g_cgi_path + string_list_join(list, "/")) )
			return HEAD_SEND_OK();

		return SEND_RESPONSE(http::hs_not_found);
	}

	else if( list[0] == "cgi-bin" )
	{
		list.pop_front();
		plugins_service(socket, request, response, string_list_join(list, "/"));
	}

	else if( fs::exists(g_resource_path + url_name) )
		SEND_RESPONSE(http::hs_not_found);
	else
		HEAD_SEND_OK();
}

static void DELETE(http::request &request, http::response &response, tcp::socket &socket)
{
	_PARSING(request);
	SEND_RESPONSE(http::hs_not_implemented);
}

static void OPTIONS(http::request &request, http::response &response, tcp::socket &socket)
{
	SEND_RESPONSE(http::hs_service_unavailable);
}

static void CONNECT(http::request &request, http::response &response, tcp::socket &socket)
{
	SEND_RESPONSE(http::hs_service_unavailable);
}

static void TRACH(http::request &request, http::response &response, tcp::socket &socket)
{
	SEND_RESPONSE(http::hs_service_unavailable);
}

}} //namespace gts::web
