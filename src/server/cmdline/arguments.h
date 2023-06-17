#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "gts/args_parser.h"

namespace gts { namespace cmdline
{

#define GC_SA_START    "start"
#define GC_SA_CFPATH   "-f"
#define GC_SA_DAEMON   "-d"
#define GC_SA_INSNAME  "-i"
#define GC_SA_STOP     "stop"
#define GC_SA_RESTART  "restart"
#define GC_SA_STATUS   "status"
#define GC_SA_VASUS    "--view-subserver-all"
#define GC_SA_VRSUS    "--view-subserver"
#define GC_SA_STSSA    "--start-subserver-all"
#define GC_SA_SPSSA    "--stop-subserver-all"
#define GC_SA_STSS     "--start-subserver"
#define GC_SA_SPSS     "--stop-subserver"

args_parser::arguments argument_check(int argc, const char *argv[], string_list &others);

}} //namespace gts::cmdline


#endif //ARGUMENTS_H
