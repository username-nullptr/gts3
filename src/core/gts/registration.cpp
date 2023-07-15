#include "registration.h"

namespace gts { namespace extension
{

std::set<const void*> registration::g_func_set;

std::size_t registration::g_gfs_counter;

}} //namespace gts::extension
