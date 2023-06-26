#include "registration.h"

namespace gts { namespace web
{

std::unordered_map<rttr::type::type_id, rttr::variant> registration::g_obj_hash;

std::unordered_map<std::string, registration::service_array> registration::g_path_hash;

std::unordered_map<std::uintptr_t, std::string> registration::g_id_hash;

}} //namespace gts::web
