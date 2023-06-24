#include "service_io.h"
#include "app_info.h"

#include "gts/web/config_key.h"
#include "gts/algorithm.h"
#include "gts/settings.h"

namespace gts { namespace web
{

static std::string g_resource_path = _GTS_WEB_DEFAULT_RC_PATH;

service_io::service_io(tcp_socket_ptr socket, http::request &request) :
	socket(socket), request(request), response(request.version)
{
	response.set_header("server", "gts3");

	if( request.version == "1.1" )
	{
		if( not request.keep_alive )
			response.set_header("connection", "close");
	}
}

void service_io::init()
{
	auto &_settings = settings::global_instance();
	g_resource_path = _settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_RC_PATH, g_resource_path);

	g_resource_path = appinfo::absolute_path(g_resource_path);
	if( not ends_with(g_resource_path, "/") )
		g_resource_path += "/";
}

std::string service_io::resource_path()
{
	return g_resource_path;
}

inline void service_io::write_some(const char *buf)
{
	write_some(buf, std::strlen(buf));
}

void service_io::write_some(const char *buf, std::size_t size)
{
	asio::error_code error;
	while( size > 0 )
	{
		auto rs = socket->write_some(buf, size, error);
		size -= rs;
		buf += rs;
	}
}

void service_io::write_some(const std::string &buf)
{
	asio::error_code error;
	const char *view = buf.c_str();
	auto size = buf.size();

	while( size > 0 )
	{
		auto rs = socket->write_some(view, size, error);
		size -= rs;
		view += rs;
	}
}

void service_io::return_to_null(http::status status)
{
	response.set_status(status);

	auto body = fmt::format("{} ({})", http::status_description(status), status);
	response.set_header("content-length", body.size());

	write_some(response.to_string() + body);
	if( not request.keep_alive )
		socket->close(true);
}

}} //namespace gts::web
