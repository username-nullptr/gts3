#if defined(__WINNT__) || defined(_WINDOWS)

#include "app_core_p.h"
#include "app_info.h"
#include "gts/log.h"

#include <cppfilesystem>
#include <iostream>
#include <fstream>
#include <thread>

namespace gts { namespace cmdline
{

args_parser::arguments startup(int argc, const char* argv[])
{

}

void app_unlock()
{

}

void stop_app(bool noreturn)
{

}

}} // namespace gts::cmdline

#endif //__WINNT__ || _WINDOWS
