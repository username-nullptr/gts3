#include "service.h"

namespace gts { namespace web
{

std::map<std::string, std::string> cgi_service_config::cgi_env;

void cgi_service_config::init()
{
	auto env_list = string_split(settings::global_instance().read<std::string>(SINI_GROUP_WEB, SINI_WEB_CGI_ENV), ";");
	if( env_list.empty() )
		return ;

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
}

}} //namespace gts::web
