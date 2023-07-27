#include "app_info.h"
#include "gts/algorithm.h"
#include <iostream>

namespace gts { namespace appinfo
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

std::string lock_file_name()
{
	return tmp_dir_path() + "pid.lock";
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
			auto tmp = getenv("HOME");
			if( tmp == nullptr )
			{
				std::cerr << "System environment 'HOME' is null." << std::endl;
				abort();
			}
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

bool is_absolute_path(const std::string &path)
{
	return str_starts_with(path, "/")
#ifdef __unix__
			or str_starts_with(path, "~/")
#endif //unix
			;
}

}} //namespace gts::appinfo
