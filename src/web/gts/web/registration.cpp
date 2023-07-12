#include "registration.h"

namespace gts { namespace web
{

std::set<const void*> registration::g_global_func_set;

std::size_t registration::g_ggfs_counter;

std::unordered_map<std::string, registration::service_array> registration::g_path_hash;

std::map<std::string, registration::service> registration::g_filter_path_map;

}} //namespace gts::web
