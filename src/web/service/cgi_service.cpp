#include "service.h"

#ifdef GTS_ENABLE_SSL
# include "gts/gts_config_key.h"
#endif //ssl

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

#ifdef GTS_ENABLE_SSL
	auto crt_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_CRT_FILE, _GTS_SSL_CRT_DEFAULT_FILE);
	if( not crt_file.empty() )
	{
		crt_file = appinfo::absolute_path(crt_file);
		cgi_env.emplace("SSL_CRT_FILE", crt_file);
	}

	auto key_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_KEY_FILE, _GTS_SSL_KEY_DEFAULT_FILE);
	if( not key_file.empty() )
	{
		key_file = appinfo::absolute_path(key_file);
		cgi_env.emplace("SSL_KEY_FILE", key_file);
	}
#endif //ssl
}

}} //namespace gts::web
