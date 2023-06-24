#include "registration.h"

namespace gts { namespace web
{

std::size_t registration::g_sid = 0;

std::unordered_map<std::string, std::size_t> registration::g_gnrm_hash;

std::unordered_map<std::string, std::unordered_map<std::string, std::size_t>> registration::g_cmnrm_hash;

}} //namespace gts::web
