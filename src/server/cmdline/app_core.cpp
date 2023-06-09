#include "private/app_core_p.h"
#include "gts/log.h"

#include "interaction.h"
#include "app_info.h"
#include "global.h"

#include <cppfilesystem>
#include <iostream>
#include <map>

namespace gts { namespace cmdline
{

static void server_is_running_check();

[[noreturn]] static void short_cmd_check(const cmdline::argument_hash &args_hash);

void cmdline_handle(int argc, const char *argv[], argument_hash &args_hash)
{
	static bool isInit = false;
	if( isInit )
		log_fatal("cmdline_handle repeat call.");
	isInit = true;

	args_hash = argument_check(argc, argv);

	if( args_hash & GC_SA_START )
	{
		if( not fs::exists(appinfo::tmp_dir_path()) )
		{
			if( fs::create_directories(appinfo::tmp_dir_path()) == false )
				log_fatal("Run(tmp) diractory make failed!");
		}
		return ;
	}
	else if( args_hash.empty() or args_hash & GC_SA_DAEMON )
	{
		std::cout << "If you want to start the server, add the argument 'start'." << std::endl;
		exit(-1);
	}
	else if( args_hash & GC_SA_STOP )
	{
		if( args_hash.size() > 1 )
			log_fatal("Too many arguments.");
		server_is_running_check();
		stop_app(true); //exit
	}
	else if( args_hash & GC_SA_RESTART )
	{
		server_is_running_check();
		return stop_app(false);
	}
	short_cmd_check(args_hash);
}

/*----------------------------------------------------------------------------------------------------------------------*/

static void server_is_running_check()
{
	if( fs::exists(appinfo::lock_file_name()) )
		return ;

	std::cout << "The server is not running." << std::endl;
	exit(-1);
}

#ifdef _WINDOWS
# define _BUF_SIZE  8192
#elif defined(__unix__)
# define _BUF_SIZE  8192
#else //other os
# define _BUF_SIZE  8192
#endif //os

[[noreturn]] static void short_cmd_check(const cmdline::argument_hash &args_hash)
{
	std::string cmd_str;

	if( args_hash & GC_SA_STATUS )
		cmd_str = CCMD_VSS;
	else if( args_hash & GC_SA_VASUS )
		cmd_str = CCMD_VASS;
	else if( args_hash & GC_SA_VRSUS )
		cmd_str = CCMD_VRSS;
	else if( args_hash & GC_SA_STSSA )
		cmd_str = CCMD_STSSA;
	else if( args_hash & GC_SA_SPSSA )
		cmd_str = CCMD_SPSSA;
	else if( args_hash & GC_SA_STSS )
		cmd_str = CCMD_STSS + args_hash.at(GC_SA_STSS);
	else if( args_hash & GC_SA_SPSS )
		cmd_str = CCMD_SPSS + args_hash.at(GC_SA_SPSS);
	else
	{
		std::cerr << "Invalid arguments." << std::endl;
		exit(-1);
	}
	if( args_hash.size() > 1 )
	{
		std::cerr << "Too many arguments." << std::endl;
		exit(-1);
	}
	server_is_running_check();

	interaction _interaction(false);
	if( _interaction.open() == false )
		log_fatal("cmd line interaction open failed.");

	int res = _interaction.write(cmd_str.c_str(), cmd_str.size(), 5000);
	if( res == 0 )
	{
		std::cerr << "Error: request operation timeout." << std::endl;
		exit(-1);
	}

	char buf[_BUF_SIZE] = "";
	res = _interaction.read(buf, _BUF_SIZE, 5000);

	if( res < 0 )
		exit(res);
	else if( res == 0 )
	{
		std::cerr << "Error: wait for reply operation timeout." << std::endl;
		exit(-1);
	}

	std::cout << buf << std::endl;
	exit(0);
}

}} //namespace gts::cmdline
