#ifndef STARTUP_INIT_H
#define STARTUP_INIT_H

#include "cmdline/arguments.h"

namespace gts { namespace cmdline
{

argument_hash startup(int argc, const char *argv[]);

void app_unlock();

void stop_app(bool noreturn);

}} //namespace gts::cmdline


#endif //STARTUP_INIT_H
