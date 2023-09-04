#ifndef GTS_CHRONO_H
#define GTS_CHRONO_H

#include <gts/cplusplus.hpp>
#include <chrono>

#if GTS_CPLUSPLUS < 202002L

namespace std { namespace chrono
{

using days   = std::chrono::duration<int64_t, std::ratio<86400>>;
using weeks  = std::chrono::duration<int64_t, std::ratio<604800>>;
using years  = std::chrono::duration<int64_t, std::ratio<31556952>>;
using months = std::chrono::duration<int64_t, std::ratio<2629746>>;

}} //namespace std::chrono

#endif //C++20


#endif //GTS_CHRONO_H
