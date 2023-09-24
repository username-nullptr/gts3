#include "application.h"
#include "private/app_info.h"
#include "gts/algorithm.h"
#include "gts/log.h"

namespace gts { namespace app
{

static std::string g_instace_name = "gts";

std::string set_instance_name(const std::string &name)
{
	g_instace_name = str_trimmed(name);
	if( g_instace_name.empty() )
		g_instace_name = "gts";
	else
	{
		for(auto &c : g_instace_name)
		{
			if( c != '/' and c != '\\' )
				continue;
			g_instace_name = "gts";
			break;
		}
	}
	return g_instace_name;
}

std::string instance_name()
{
	return g_instace_name;
}

std::string dir_path()
{
	auto file_name = file_path();
	auto index = file_name.find_last_of('/');
	if( index == std::string::npos or index == file_name.size() - 1 )
		return "./";
	return file_name.erase(index+1);
}

bool is_absolute_path(const std::string &path)
{
	return str_starts_with(path, "/")
#ifdef __unix__
			or str_starts_with(path, "~/")
#endif //unix
			;
}

using optional_string = cpp::optional<std::string>;

optional_string getenv(const std::string &key)
{
	auto value = ::getenv(key.c_str());
	return value? optional_string(value) : optional_string();
}

bool setenv(const std::string &key, const std::string &value, bool overwrite)
{
	if( ::setenv(key.c_str(), value.c_str(), overwrite) != 0 )
	{
		gts_log_error("gts::appinfo::setenv: setenv: {}. ({})", strerror(errno), errno);
		return false;
	}
	return true;
}

bool unsetenv(const std::string &key)
{
	if( ::unsetenv(key.c_str()) != 0 )
	{
		gts_log_error("gts::appinfo::unsetenv: unsetenv: {}. ({})", strerror(errno), errno);
		return false;
	}
	return true;
}

}} //namespace gts::app
