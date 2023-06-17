#ifndef APP_CORE_P_H
#define APP_CORE_P_H

#include "cmdline/app_core.h"
#include "cmdline/arguments.h"

namespace gts { namespace cmdline
{

void cmdline_handle(int argc, const char *argv[], args_parser::arguments &args_hash);

}} //namespace gts::cmdline


#endif //APP_CORE_P_H
