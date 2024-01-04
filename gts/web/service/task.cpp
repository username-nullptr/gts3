
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

#include "task.h"
#include "service_io.h"
#include "plugins_service.h"
#include "cgi/service.h"

#include "gts/settings.h"
#include "gts/application.h"
#include "gts/http/formatter.h"

#include "gts/web/config_key.h"
#include "gts/web/thread_pool.h"
#include <cppfilesystem>

GTS_WEB_NAMESPACE_BEGIN

task::task(tcp_socket_ptr socket) :
	m_socket(std::move(socket))
{

}

static std::string g_default_resource = "/index.html";

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
		gts_log_fatal("access extensions is empty.");

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
	str_replace(str, "\\", "/");

	if( not str_starts_with(str, "/") )
		str = "/" + str;

	if( str == "/" )
		gts_log_fatal("Default resource or access extensions is empty.");
}

void task::init()
{
	auto &_setting = settings::global_instance();

	g_default_resource = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_DEFAULT_RESOURCE, g_default_resource);
	access_check(g_default_resource);

	if( fs::is_directory(g_default_resource) )
		gts_log_fatal("Default resource is a directory.");

	g_cgi_access = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_ACCESS, g_cgi_access);
	access_check(g_cgi_access);

	g_cgi_path = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_PATH, g_cgi_path);
	g_cgi_path = app::absolute_path(g_cgi_path);

	if( not str_ends_with(g_cgi_path, "/") )
		g_cgi_path += "/";

	if( fs::path(g_cgi_path) == fs::path(resource_root()) )
		gts_log_fatal("The paths of 'cgi' and 'resource' cannot be the same.");

	if( not fs::exists(g_cgi_path) )
		fs::create_directories(g_cgi_path);

	gts_log_debug("web task config:\n"
			  "    default_resource: {}\n"
			  "    cgi_access      : {}\n"
			  "    cgi_path        : {}",
			  g_default_resource,
			  g_cgi_access,
			  g_cgi_path);
	cgi_service::init();
}

static void CALL_GET    (service_io &sio);
static void CALL_POST   (service_io &sio);
static void CALL_PUT    (service_io &sio);
static void CALL_HEAD   (service_io &sio);
static void CALL_DELETE (service_io &sio);
static void CALL_OPTIONS(service_io &sio);
static void CALL_CONNECT(service_io &sio);
static void CALL_TRACH  (service_io &sio);

bool task::run(http::service_context_ptr context)
{
	m_socket->non_blocking(false);

	service_io sio(*context);
	auto method = context->request().method();

	gts_log_debug("URL: '{}' ({:s})", context->request().path(), method);
	coro_run_on(thread_pool());

	if(      method == http::GET     ) CALL_GET    (sio);
	else if( method == http::POST    ) CALL_POST   (sio);
	else if( method == http::HEAD    ) CALL_HEAD   (sio);
	else if( method == http::PUT     ) CALL_PUT    (sio);
	else if( method == http::DELETE  ) CALL_DELETE (sio);
	else if( method == http::OPTIONS ) CALL_OPTIONS(sio);
	else if( method == http::CONNECT ) CALL_CONNECT(sio);
	else if( method == http::TRACH   ) CALL_TRACH  (sio);

	else sio.response().write_default(http::hs_bad_request);
	if( not context->request().keep_alive() )
		m_socket->close(true);

	coro_run_on(m_socket->native().get_executor());
	return m_cancel or context->is_valid();
}

void task::cancel()
{
	m_cancel = true;
}

bool task::is_cancel() const
{
	return m_cancel;
}

/*--------------------------------------------------------------------------------------------------------------------*/

#define TASK_DO_PARSING(sio) \
({ \
	sio.url_name = sio.request().path(); \
	if( sio.url_name.empty() ) \
		sio.url_name = "/"; \
	auto ps = std::make_shared<plugins_service>(sio); \
	if( ps->call() ) \
		return ; \
	auto pos = sio.url_name.find('/',1); \
	if( pos != std::string::npos ) { \
		auto prefix = sio.url_name.substr(0,pos); \
		if( prefix == g_cgi_access ) { \
			auto tmp = sio.url_name; \
			sio.url_name = g_cgi_path + sio.url_name.substr(pos+1); \
			cgi_service cs(sio); \
			auto file_name = cs.exists(); \
			if( not file_name.empty() and ps->call_filter() == false ) { \
				if( cs.call(file_name) ) \
					return ; \
			} \
			sio.url_name = tmp; \
		} \
	} \
	ps; \
})

static void CALL_GET(service_io &sio)
{
	auto _ps = TASK_DO_PARSING(sio);
	if( sio.url_name == "/" )
		sio.url_name = g_default_resource;
	sio.url_name = resource_root() + sio.url_name;

	if( fs::is_directory(sio.url_name) )
		return sio.return_to_null(http::hs_not_found);

	else if( not _ps->call_filter() )
		sio.response().write_file(sio.url_name);
}

static void CALL_POST(service_io &sio)
{
	TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

static void CALL_PUT(service_io &sio)
{
	TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

static void CALL_HEAD(service_io &sio)
{
	sio.url_name = sio.request().path();
	if( sio.url_name.empty() )
		sio.url_name = "/";

	if( plugins_service(sio).exists() )
		return sio.return_to_null();

	auto pos = sio.url_name.find('/',1);
	if( pos != std::string::npos )
	{
		auto prefix = sio.url_name.substr(0,pos);
		if( prefix == g_cgi_access )
		{
			if( fs::exists(g_cgi_path + sio.url_name.substr(pos+1)) )
				return sio.return_to_null();
		}
	}
	if( fs::exists(resource_root() + sio.url_name) )
		sio.return_to_null(http::hs_not_found);
	else
		sio.return_to_null();
}

static void CALL_DELETE(service_io &sio)
{
	TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

static void CALL_OPTIONS(service_io &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

static void CALL_CONNECT(service_io &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

static void CALL_TRACH(service_io &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

GTS_WEB_NAMESPACE_END
