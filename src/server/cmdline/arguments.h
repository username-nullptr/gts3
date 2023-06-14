#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>
#include <map>

namespace gts { namespace cmdline
{

#define GC_SA_START    "start"
#define GC_SA_CFPATH   "-f"
#define GC_SA_DAEMON   "-d"
#define GC_SA_INSNAME  "-i"
#define GC_SA_STOP     "stop"
#define GC_SA_RESTART  "restart"
#define GC_SA_STATUS   "stat"
#define GC_SA_VASUS    "--view-subserver-all"
#define GC_SA_VRSUS    "--view-subserver"
#define GC_SA_STSSA    "--start-subserver-all"
#define GC_SA_SPSSA    "--stop-subserver-all"
#define GC_SA_STSS     "--start-subserver"
#define GC_SA_SPSS     "--stop-subserver"

typedef std::map<std::string, std::string>  argument_hash;

argument_hash argument_check(int argc, const char *argv[]);

bool operator&(const argument_hash &args_hash, const std::string &key);
bool operator&(const std::string &key, const argument_hash &args_hash);

}} //namespace gts::cmdline


#endif //ARGUMENTS_H
