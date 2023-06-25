#include "service_io.h"
#include "app_info.h"

#include "gts/web/config_key.h"
#include "gts/algorithm.h"
#include "gts/settings.h"

namespace gts { namespace web
{

static std::string g_resource_path = _GTS_WEB_DEFAULT_RC_PATH;

service_io::service_io(tcp_socket_ptr socket, http::request &request) :
	request(request), response(std::move(socket), request.version)
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

void service_io::return_to_null(http::status status)
{
	response.set_status(status);
	response.write(fmt::format("{} ({})", http::status_description(status), status));

	if( not request.keep_alive )
		response.close(true);
}

}} //namespace gts::web
