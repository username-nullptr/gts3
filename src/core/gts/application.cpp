#include "application.h"
#include "app_info.h"

namespace gts { namespace app
{

std::string instance_name()
{
	return appinfo::instance_name();
}

std::string file_path()
{
	return appinfo::file_path();
}

std::string dir_path()
{
	return appinfo::dir_path();
}

std::string current_directory()
{
	return appinfo::current_directory();
}

std::string absolute_path(const std::string &path)
{
	return appinfo::absolute_path(path);
}

bool is_absolute_path(const std::string &path)
{
	return appinfo::is_absolute_path(path);
}

}} //namespace gts::app
