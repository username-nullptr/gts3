#include "service.h"
#include "gts/gts_config_key.h"

namespace gts { namespace web
{

std::map<std::string, std::string> cgi_service_config::cgi_env;

void cgi_service_config::init()
{
	auto &_settings = settings::global_instance();
	auto env_list = string_split(_settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_ENV), ";");

	for(auto &env : env_list)
	{
		auto pos = env.find("=");

		if( pos == std::string::npos )
			cgi_env.emplace(trimmed(env), "");

		else {
			cgi_env.emplace(to_upper(trimmed(env.substr(0, pos))),
							to_upper(trimmed(env.substr(pos + 1))));
		}
	}

	if( _settings.read<bool>(SINI_GROUP_GTS, SINI_GTS_ENABLE_SSL) == false )
		return ;

	auto crt_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_CRT_FILE, _GTS_SSL_CRT_DEFAULT_FILE);
	if( not crt_file.empty() )
	{
		if( not starts_with(crt_file, "/") )
			crt_file = appinfo::current_directory() + crt_file;
		cgi_env.emplace("SSL_CRT_FILE", crt_file);
	}

	auto key_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_KEY_FILE, _GTS_SSL_KEY_DEFAULT_FILE);
	if( not key_file.empty() )
	{
		if( not starts_with(key_file, "/") )
			key_file = appinfo::current_directory() + key_file;
		cgi_env.emplace("SSL_KEY_FILE", key_file);
	}
}

}} //namespace gts::web
