#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>
#include <map>

namespace gts { namespace cmdline
{

enum argument
{
	sa_start = 1,
	sa_cfpath,
	sa_daemon,
	sa_stop,
	sa_restart,
	sa_status,
	sa_vasus,
	sa_vrsus,
	sa_stssa,
	sa_spssa,
	sa_stss,
	sa_spss,
	sa_other
};
typedef std::map<argument, std::string>  argument_hash;

argument_hash argument_check(int argc, const char *argv[]);

bool operator&(const argument_hash &args_hash, argument key);
bool operator&(argument key, const argument_hash &args_hash);

}} //namespace gts::cmdline


#endif //ARGUMENTS_H
