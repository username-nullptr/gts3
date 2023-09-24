#include "registration.h"

namespace gts
{

std::set<const void*> registration_base::g_func_set;

std::size_t registration_base::g_gfs_counter = 0;

registration_base::~registration_base()
{

}

std::unordered_map<rttr::type, rttr::variant> registration::g_obj_hash;

} //namespace gts
