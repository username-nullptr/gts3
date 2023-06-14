#ifdef __unix__

#include "app_info.h"
#include "gts/algorithm.h"

#include <fmt/format.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

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

std::string file_path()
{
	char exe_name[1024] = "";
	if( readlink("/proc/self/exe", exe_name, sizeof(exe_name)) < 0 )
		std::cerr << fmt::format("*** Error: appinfo::file_path: readlink: {}. ({})", strerror(errno), errno) << std::endl;
	return exe_name;
}

std::string dir_path()
{
	auto file_name = file_path();
	auto index = file_name.find_last_of('/');
	if( index == std::string::npos or index == file_name.size() - 1 )
		return "./";
	return file_name.erase(index+1);
}

#define APP_TMP_DIR_PATH  "/tmp/gts3/"

std::string tmp_dir_path()
{
	return APP_TMP_DIR_PATH + g_instace_name + "/";
}

std::string lock_file_name()
{
	return tmp_dir_path() + "pid.lock";
}

std::string current_directory()
{
	char buf[1024] = "";
	if( getcwd(buf, sizeof(buf)) == nullptr )
		std::cerr << fmt::format("*** Error: appinfo::current_directory: getcwd: {}. ({})", strerror(errno), errno) << std::endl;
	std::string str(buf);
	if( not ends_with(str, "/") )
		str += "/";
	return str;
}

bool set_current_directory(const std::string &path)
{
	if( chdir(path.c_str()) < 0 )
	{
		std::cerr << fmt::format("*** Error: appinfo::set_current_directory: chdir: {}. ({})", strerror(errno), errno) << std::endl;
		return false;
	}
	return true;
}

}} //namespace gts::appinfo

#endif //__unix__
