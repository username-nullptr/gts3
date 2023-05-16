#include "arguments.h"
#include "gts_log.h"

#include <iostream>
#include <cstring>

namespace gts { namespace cmdline
{

#define ERROR_EXIT(msg) \
({ \
	std::cerr << msg << std::endl; \
	exit(-1); \
})

#define ARGC_EQ_1(ope) \
({ \
	if( argc == 1 ) { \
		ope; \
	} else { \
		ERROR_EXIT("Too many arguments."); \
	} \
	break; \
})

#define ARGC_EQ_2(ope) \
({ \
	if( argc == 2 ) { \
		if( i == 0 ) { \
			ope; \
		} else { \
			ERROR_EXIT("Invalid arguments."); \
		} \
		break; \
	} else if( argc == 1 ) { \
		ERROR_EXIT("Invalid argument."); \
	} else { \
		ERROR_EXIT("Too many arguments."); \
	} \
})

[[noreturn]] static void printVersion();
[[noreturn]] static void printHelp();

// Modify or extend this function
static void __argumentCheck(int argc, const char *argv[], argument_hash &args_hash)
{
	for(int i=0 ;i<argc; i++)
	{
		if( strcmp(argv[i], "stop") == 0 )
			args_hash.emplace(sa_stop, std::string());

		else if( strcmp(argv[i], "start") == 0 )
			args_hash.emplace(sa_start, std::string());

		else if( strcmp(argv[i], "restart") == 0 )
			args_hash.emplace(sa_restart, std::string());

		else if( strcmp(argv[i], "-d") == 0 )
			args_hash.emplace(sa_daemon, std::string());

		else if( strcmp(argv[i], "-f") == 0 )
		{
			if( i + 1 >= argc )
				ERROR_EXIT("Invalid arguments.");
			args_hash.emplace(sa_cfpath, argv[++i]);
		}
		else if( strcmp(argv[i], "-df") == 0 )
		{
			if( i + 1 >= argc )
				ERROR_EXIT("Invalid arguments.");
			args_hash.emplace(sa_cfpath, argv[++i]);
			args_hash.emplace(sa_daemon, std::string());
		}

		///////////////////////////////////////////////////////////////////////////////////////
		else if( strcmp(argv[i], "stat") == 0 )
			ARGC_EQ_1(args_hash.emplace(sa_status, std::string()));

		else if( strcmp(argv[i], "--start-subserver-all") == 0 )
			ARGC_EQ_1(args_hash.emplace(sa_stssa, std::string()));

		else if( strcmp(argv[i], "--stop-subserver-all") == 0 )
			ARGC_EQ_1(args_hash.emplace(sa_spssa, std::string()));

		else if( strcmp(argv[i], "--view-subserver-all") == 0 )
			ARGC_EQ_1(args_hash.emplace(sa_vasus, std::string()));

		else if( strcmp(argv[i], "--view-subserver") == 0 )
			ARGC_EQ_1(args_hash.emplace(sa_vrsus, std::string()));

		else if( strcmp(argv[i], "--version") == 0 )
			ARGC_EQ_1(printVersion());

		else if( strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0 )
			ARGC_EQ_1(printHelp());

		///////////////////////////////////////////////////////////////////////////////////////
		else if( strcmp(argv[i], "--start-subserver") == 0 )
			ARGC_EQ_2(args_hash.emplace(sa_stss, argv[++i]));

		else if( strcmp(argv[i], "--stop-subserver") == 0 )
			ARGC_EQ_2(args_hash.emplace(sa_spss, argv[++i]));

		///////////////////////////////////////////////////////////////////////////////////////
		else ERROR_EXIT("Invalid arguments.");
	}
}

argument_hash argument_check(int argc, const char *argv[])
{
	argument_hash sa;
	if( argc <= 1 )
		return sa;

	__argumentCheck(argc - 1, argv + 1, sa);
	return sa;
}

bool operator&(const argument_hash &args_hash, argument key)
{
	return args_hash.find(key) != args_hash.end();
}

bool operator&(argument key, const argument_hash &args_hash)
{
	return args_hash.find(key) != args_hash.end();
}

/*----------------------------------------------------------------------------------------------------------------------*/

#ifdef __llvm__
# define IS_CLANG  "LLVM-"
#else //no llvm
# define IS_CLANG  ""
#endif //Clang

[[noreturn]] static void printVersion()
{
	std::cout << "Version=" << GTS_VERSION_STR << std::endl;

#ifdef __GNUC__
	std::cout << "Compiler: " IS_CLANG "GCC " << __VERSION__ << std::endl;
#elif defined(_MSC_VER)
	std::cout << "Compiler: " IS_CLANG "MSVC " << _MSC_VER << std::endl;
#endif //GCC

	std::cout << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
	::exit(0);
}

[[noreturn]] static void printHelp()
{
	std::cout << "GTS version=" << GTS_VERSION_STR                                                                    << std::endl;
	std::cout << "Description of command line parameters:"                                                            << std::endl;
	std::cout << "  stop                               : Stop the server."                                            << std::endl;
	std::cout << "  restart                            : Restart the server."                                         << std::endl;
	std::cout << "  -d                                 : Start as a daemon process."                                  << std::endl;
	std::cout << "  -f file                            : Specify the configuration file (default is '/etc/gts.ini')." << std::endl;
	std::cout << "  stat                               : Viewing server status."                                      << std::endl;
	std::cout << "  --version                          : Viewing server version."                                     << std::endl;
	std::cout << "  -h or --help                       : Viewing help."                                               << std::endl;
	std::cout << "  --view-subserver-all               : Viewing all subservers."                                     << std::endl;
	std::cout << "  --view-subserver                   : Viewing running subservers."                                 << std::endl;
	std::cout << "  --start-subserver-all              : Start all subservers."                                       << std::endl;
	std::cout << "  --start-subserver <name0,name1...> : Start subservices in the list."                              << std::endl;
	std::cout << "  --stop-subserver-all               : Stop all subservers."                                        << std::endl;
	std::cout << "  --stop-subserver <name0,name1...>  : Stop subservices in the list."                               << std::endl;
	exit(0);
}

}} //namespace gts::cmdline
