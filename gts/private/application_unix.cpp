#ifdef __unix__

#include "app_info.h"
#include "gts/algorithm.h"
#include "gts/log.h"

#include <cppformat>
#include <iostream>
#include <cstring>

#include <unistd.h>

extern char **environ;

namespace gts { namespace app
{

std::string file_path()
{
	char exe_name[1024] = "";
	if( readlink("/proc/self/exe", exe_name, sizeof(exe_name)) < 0 )
		gts_log_error("gts::appinfo::file_path: readlink: {}. ({})", strerror(errno), errno);
	return exe_name;
}

bool set_current_directory(const std::string &path)
{
	if( chdir(path.c_str()) < 0 )
	{
		gts_log_error("gts::appinfo::set_current_directory: chdir: {}. ({})", strerror(errno), errno);
		return false;
	}
	return true;
}

std::string current_directory()
{
	char buf[1024] = "";
	if( getcwd(buf, sizeof(buf)) == nullptr )
		gts_log_error("gts::appinfo::current_directory: getcwd: {}. ({})", strerror(errno), errno);
	std::string str(buf);
	if( not str_ends_with(str, "/") )
		str += "/";
	return str;
}

std::string absolute_path(const std::string &path)
{
	auto result = path;
#ifdef _WINDOWS
	// ......
#else
	if( not str_starts_with(result, "/") )
	{
		if( str_starts_with(result, "~/") )
		{
			auto tmp = ::getenv("HOME");
			if( tmp == nullptr )
				gts_log_fatal("System environment 'HOME' is null.");

			std::string home(tmp);
			if( not str_ends_with(home, "/") )
				home += "/";
			result = home + result.erase(0,2);
		}
		else
			result = dir_path() + result;
	}
#endif //unix
	return result;
}

using envs_t = std::map<std::string, std::string>;

envs_t getenvs()
{
	envs_t envs;
	for(int i=0; environ[i]!=nullptr; i++)
	{
		std::string tmp = environ[i];
		auto pos = tmp.find("=");

		if( pos == tmp.npos )
			envs.emplace(tmp, "");
		else
			envs.emplace(tmp.substr(0,pos), tmp.substr(pos+1));
	}
	return envs;
}

}} //namespace gts::app

#endif //__unix__
