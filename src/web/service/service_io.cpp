#include "service_io.h"
#include "gts/web_config_key.h"
#include "gts/algorithm.h"
#include "settings.h"
#include "app_info.h"

namespace gts { namespace web
{

std::string service_io_config::resource_path = _GTS_WEB_DEFAULT_RC_PATH;

void service_io_config::init()
{
	auto &_settings = settings::global_instance();
	resource_path = _settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_RC_PATH, resource_path);

	resource_path = appinfo::absolute_path(resource_path);
	if( not ends_with(resource_path, "/") )
		resource_path += "/";
}

}} //namespace gts::web
