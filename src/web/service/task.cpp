#include "task.h"
#include "service.h"
#include "service_io.h"

#include "gts/log.h"
#include "gts/settings.h"
#include "gts/algorithm.h"
#include "gts/tcp_socket.h"
#include "gts/application.h"
#include "gts/web/config_key.h"
#include "gts/web/thread_pool.h"

#include <cppfilesystem>

namespace gts { namespace web
{

task::task(tcp_socket_ptr socket) :
	m_socket(std::move(socket))
{

}

static std::string g_default_resource = "/index.html";

static std::string g_plugins_access = "/plugin-lib";

static std::string g_cgi_access = "/cgi-bin";

static std::string g_cgi_path = _GTS_WEB_DEFAULT_CGI_PATH;

static void trimmed_path(std::string &str)
{
	std::size_t start = 0;
	for(; start<str.size(); start++)
	{
		if( str[start] == '/' or str[start] == '\\' )
			continue;
		else if( start > 0 )
			start--;
		break;
	}
	if( start == str.size() )
		log_fatal("access extensions is empty.");

	std::size_t end = start + 1;
	for(; end < str.size() and
		str[end] != '/' and
		str[end] != '\\'
		; end++);
	str = str.substr(start, end - start);
}

static inline void access_check(std::string &str)
{
	trimmed_path(str);
	replace(str, "\\", "/");

	if( not starts_with(str, "/") )
		str = "/" + str;

	if( str == "/" or str == "\\" )
		log_fatal("Default resource or access extensions is empty.");
}

void task::init()
{
	auto &_setting = settings::global_instance();

	g_default_resource = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_DEFAULT_RESOURCE, g_default_resource);
	access_check(g_default_resource);

	g_plugins_access = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_PLUGINS_ACCESS, g_plugins_access);
	access_check(g_plugins_access);

	g_cgi_access = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_ACCESS, g_cgi_access);
	access_check(g_cgi_access);

	if( g_default_resource == g_plugins_access or g_default_resource == g_cgi_access or g_plugins_access == g_cgi_access )
		log_fatal("Config error: 'default_resource', 'plugins_access', 'cgi_access' cannot be equal.");

	g_cgi_path = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_PATH, g_cgi_path);
	g_cgi_path = app::absolute_path(g_cgi_path);

	if( not ends_with(g_cgi_path, "/") )
		g_cgi_path += "/";

	if( not fs::exists(g_cgi_path) )
		fs::create_directories(g_cgi_path);

	log_debug("web task config:\n"
			  "    default_resource: {}\n"
			  "    plugins_access  : {}\n"
			  "    cgi_access      : {}\n"
			  "    cgi_path        : {}",
			  g_default_resource,
			  g_plugins_access,
			  g_cgi_access,
			  g_cgi_path);
	service::init();
}

void task::exit()
{
	service::exit();
}

std::string task::view_status()
{
	return plugin_service::view_status();
}

void task::start(std::shared_ptr<http::request> request)
{
	m_request = std::move(request);
	m_socket->non_blocking(false);

	asio::post(thread_pool(), [this]
	{
		run();
		io_context().post([this]
		{
			if( m_call_back )
			{
				m_request.reset();
				m_call_back();
			}
		});
	});
}

void task::async_wait_next(std::function<void()> call_back)
{
	assert(call_back != nullptr);
	m_call_back = std::move(call_back);
}

void task::cancel()
{
	m_call_back = nullptr;
}

static void _GET    (service_io &sio);
static void _POST   (service_io &sio);
static void _PUT    (service_io &sio);
static void _HEAD   (service_io &sio);
static void _DELETE (service_io &sio);
static void _OPTIONS(service_io &sio);
static void _CONNECT(service_io &sio);
static void _TRACH  (service_io &sio);

void task::run()
{
	auto method = m_request->method;
	log_debug() << "URL:" << m_request->path << method;

	service_io sio(m_socket, *m_request);

	if(      method == http::GET     ) _GET    (sio);
	else if( method == http::POST    ) _POST   (sio);
	else if( method == http::HEAD    ) _HEAD   (sio);
	else if( method == http::PUT     ) _PUT    (sio);
	else if( method == http::DELETE  ) _DELETE (sio);
	else if( method == http::OPTIONS ) _OPTIONS(sio);
	else if( method == http::CONNECT ) _CONNECT(sio);
	else if( method == http::TRACH   ) _TRACH  (sio);

	else {
		m_socket->write_some("HTTP/1.1 400 Bad Request\r\n"
							 "content-length: 11\r\n"
							 "connection: close\r\n"
							 "\r\n"
							 "Bad Request");
	}
	if( not m_request->keep_alive )
		m_socket->close(true);
}

#define _TASK_DO_PARSING(_sio) \
({ \
	sio.url_name = _sio.request.path; \
	if( sio.url_name.empty() or sio.url_name == "/" ) \
		sio.url_name = g_default_resource; \
	else if( sio.url_name != g_default_resource ) { \
		auto pos = sio.url_name.find("/", 1); \
		if( pos != std::string::npos ) { \
			auto prefix = sio.url_name.substr(0,pos); \
			if( prefix == g_cgi_access ) { \
				sio.url_name = g_cgi_path + sio.url_name.substr(pos+1); \
				return service::call_cgi_service(sio); \
			} else if ( prefix == g_plugins_access ) { \
				sio.url_name.erase(0, pos+1); \
				return service::call_plugin_service(sio); \
			} \
		} \
	} \
	if( fs::is_directory(sio.url_name) ) \
		return _sio.return_to_null(http::hs_not_found); \
	sio.url_name; \
})

static void _GET(service_io &sio)
{
	sio.url_name = service_io::resource_path() + _TASK_DO_PARSING(sio);
	service::call_static_resource_service(sio);
}

static void _POST(service_io &sio)
{
	_TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

static void _PUT(service_io &sio)
{
	_TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

static void _HEAD(service_io &sio)
{
	sio.url_name = sio.request.path;
	if( sio.url_name == "/" )
		return sio.return_to_null();

	auto pos = sio.url_name.find("/", 1);

	if( pos != std::string::npos )
	{
		auto prefix = sio.url_name.substr(0,pos);
		if( prefix == g_cgi_access )
		{
			if( fs::exists(g_cgi_path + sio.url_name.substr(pos+1)) )
				return sio.return_to_null();
			return sio.return_to_null(http::hs_not_found);
		}
		else if ( prefix == g_plugins_access )
		{
			sio.url_name.erase(0, pos+1);
			return service::call_plugin_service(sio);
		}
	}

	else if( fs::exists(service_io::resource_path() + sio.url_name) )
		sio.return_to_null(http::hs_not_found);
	else
		sio.return_to_null();
}

static void _DELETE(service_io &sio)
{
	_TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

static void _OPTIONS(service_io &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

static void _CONNECT(service_io &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

static void _TRACH(service_io &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}
}} //namespace gts::web
