#if defined(__WINNT__) || defined(_WINDOWS)

#include "app_core_p.h"
#include "app_info.h"
#include "gts/log.h"

#include <cppfilesystem>
#include <iostream>
#include <fstream>
#include <thread>

GTS_CMDLINE_NAMESPACE_BEGIN

args_parser::arguments startup(int argc, const char* argv[])
{

}

void app_unlock()
{

}

void stop_app(bool noreturn)
{

}

GTS_CMDLINE_NAMESPACE_END

#endif //__WINNT__ || _WINDOWS
