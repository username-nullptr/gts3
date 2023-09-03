#ifdef __unix__

#include "app_info.h"
#include "gts/algorithm.h"
#include "gts/log.h"

#include <cppformat>
#include <iostream>
#include <cstring>

#include <unistd.h>

namespace gts { namespace appinfo
{

std::string file_path()
{
	char exe_name[1024] = "";
	if( readlink("/proc/self/exe", exe_name, sizeof(exe_name)) < 0 )
		gts_log_error("gts::appinfo::file_path: readlink: {}. ({})", strerror(errno), errno);
	return exe_name;
}

#define APP_TMP_DIR_PATH  "/tmp/gts3/"

std::string tmp_dir_path()
{
	return APP_TMP_DIR_PATH + instance_name() + "/";
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

bool set_current_directory(const std::string &path)
{
	if( chdir(path.c_str()) < 0 )
	{
		gts_log_error("gts::appinfo::set_current_directory: chdir: {}. ({})", strerror(errno), errno);
		return false;
	}
	return true;
}

}} //namespace gts::appinfo

#endif //__unix__
