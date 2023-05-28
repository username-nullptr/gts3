#include "app_info.h"
#include "gts/algorithm.h"

namespace gts { namespace appinfo
{

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
