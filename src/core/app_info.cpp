#include "app_info.h"
#include "gts/algorithm.h"

namespace gts { namespace appinfo
{

static std::string g_instace_name = "gts";

std::string set_instance_name(const std::string &name)
{
	g_instace_name = trimmed(name);
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
	if( not starts_with(result, "/") )
	{
#ifdef __unix__
		if( starts_with(result, "~/") )
		{
			std::string home = getenv("HOME");
			if( not ends_with(home, "/") )
				home += "/";
			result = home + result.erase(0,2);
		}
		else
#endif //unix
		{ result = dir_path() + result; }
	}
	return result;
}

bool is_absolute_path(const std::string &path)
{
	return starts_with(path, "/")
#ifdef __unix__
			or starts_with(path, "~/")
#endif //unix
			;
}

}} //namespace gts::appinfo
