#ifndef APP_CORE_P_H
#define APP_CORE_P_H

#include "cmdline/app_core.h"
#include "cmdline/arguments.h"

namespace gts::cmdline
{

void cmdline_handle(int argc, const char *argv[], argument_hash &args_hash);

} //namespace gts::cmdline


#endif //APP_CORE_P_H
