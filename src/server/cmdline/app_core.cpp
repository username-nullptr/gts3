#include "private/app_core_p.h"
#include "interaction.h"
#include "global.h"
#include "log.h"

#include <cppfilesystem>
#include <iostream>
#include <map>

namespace gts { namespace cmdline
{

static void start_check(const cmdline::argument_hash &args_hash);
[[noreturn]] static void short_cmd_check(const cmdline::argument_hash &args_hash);

void cmdline_handle(int argc, const char *argv[], argument_hash &args_hash)
{
	static bool isInit = false;
	if( isInit )
		log_fatal("cmdline_handle repeat call.");
	isInit = true;

	args_hash = argument_check(argc, argv);

	if( args_hash & sa_start )
	{
		start_check(args_hash);
		if( not fs::exists(appinfo::tmp_dir_path()) )
		{
			if( fs::create_directories(appinfo::tmp_dir_path()) == false )
				log_fatal("Run(tmp) diractory make failed!");
		}
		return ;
	}
	else if( args_hash.empty() or args_hash & sa_daemon )
	{
		std::cout << "If you want to start the server, add the argument 'start'." << std::endl;
		exit(-1);
	}

	if( not fs::exists(appinfo::lock_file_name()) )
	{
		std::cout << "The server is not running." << std::endl;
		exit(-1);
	}
	else if( args_hash & sa_stop )
	{
		if( args_hash.size() > 1 )
			log_fatal("Too many arguments.");
		stop_app(true); //exit
	}
	else if( args_hash & sa_restart )
	{
		start_check(args_hash);
		return stop_app(false);
	}
	short_cmd_check(args_hash);
}

/*----------------------------------------------------------------------------------------------------------------------*/

static void start_check(const cmdline::argument_hash &args_hash)
{
	if( args_hash.size() == 3 )
	{
		if( (args_hash & sa_cfpath) == false or
			(args_hash & sa_daemon) == false )
		{
			std::cerr << "Invalid arguments." << std::endl;
			exit(-1);
		}
	}
	else if( args_hash.size() > 3 )
	{
		std::cerr << "Too many arguments." << std::endl;
		exit(-1);
	}
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
	if( args_hash.size() > 1 )
	{
		std::cerr << "Too many arguments." << std::endl;
		exit(-1);
	}
	std::string cmd_str;

	if( args_hash & sa_status )
		cmd_str = CCMD_VSS;
	else if( args_hash & sa_vasus )
		cmd_str = CCMD_VASS;
	else if( args_hash & sa_vrsus )
		cmd_str = CCMD_VRSS;
	else if( args_hash & sa_stssa )
		cmd_str = CCMD_STSSA;
	else if( args_hash & sa_spssa )
		cmd_str = CCMD_SPSSA;
	else if( args_hash & sa_stss )
		cmd_str = CCMD_STSS + args_hash.at(sa_stss);
	else if( args_hash & sa_spss )
		cmd_str = CCMD_SPSS + args_hash.at(sa_spss);
	else
		log_fatal("code bug.");

	interaction _interaction(false);
	if( _interaction.open() == false )
		log_fatal("cmd line interaction open failed.");

	_interaction.write(cmd_str.c_str(), cmd_str.size());

	char buf[_BUF_SIZE] = "";
	int res = _interaction.read(buf, _BUF_SIZE);

	if( res < 0 )
		exit(res);

	std::cout << buf << std::endl;
	exit(0);
}

}} //namespace gts::cmdline
