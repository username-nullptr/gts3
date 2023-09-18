#include "registration.h"

namespace gts { namespace web
{

std::set<const void*> registration::g_global_func_set;

std::size_t registration::g_ggfs_counter = 0;

std::unordered_map<std::string, registration::service_array> registration::g_path_hash;

std::map<std::string, registration::service> registration::g_filter_path_map;

std::size_t registration::log2(http::method n)
{
	assert(n >= http::GET and n <= http::TRACH);
	if( n == http::GET )
		return 0;

	std::size_t sum = 2, res = 1;
	for(; sum!=static_cast<std::size_t>(n); sum<<=1, res++);
	return res;
}

}} //namespace gts::web
