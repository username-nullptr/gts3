#include "task.h"
#include "gts/algorithm.h"

namespace gts { namespace web
{

std::string task_config::default_resource = "/index.html";

std::string task_config::plugins_access = "/plugin-lib";

std::string task_config::cgi_access = "/cgi-bin";

std::string task_config::cgi_path = _GTS_WEB_DEFAULT_CGI_PATH;

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

void task_config::init()
{
	auto &_setting = settings::global_instance();

	default_resource = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_DEFAULT_RESOURCE, default_resource);
	access_check(default_resource);

	plugins_access = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_PLUGINS_ACCESS, plugins_access);
	access_check(plugins_access);

	cgi_access = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_ACCESS, cgi_access);
	access_check(cgi_access);

	cgi_path = _setting.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_PATH, cgi_path);
	cgi_path = appinfo::absolute_path(cgi_path);

	if( not ends_with(cgi_path, "/") )
		cgi_path += "/";

	if( not fs::exists(cgi_path) )
		fs::create_directories(cgi_path);

	log_debug("web task config:\n"
			  "    default_resource: {}\n"
			  "    plugins_access  : {}\n"
			  "    cgi_access      : {}\n"
			  "    cgi_path        : {}",
			  default_resource,
			  plugins_access,
			  cgi_access,
			  cgi_path);
	service::init();
}

void task_config::exit()
{
	service::exit();
}

std::string task_config::view_status()
{
	return plugin_service_config::view_status();
}

}} //namespace gts::web
