#include "arguments.h"
#include "server_tool.h"

#include <rttr/library.h>
#include <cppfilesystem>
#include <iostream>

#ifdef GTS_ENABLE_SSL
# include <openssl/ssl.h>
#endif //ssl

namespace gts { namespace cmdline
{

static std::list<rttr::library> g_lib_lists;

static std::string view_version(bool all = false);

args_parser::arguments argument_check(int argc, const char *argv[], string_list &others)
{
	try {
		for(auto &entry : fs::directory_iterator("./startup_extensions"))
		{
			if( not entry.is_regular_file() )
				continue;

			g_lib_lists.emplace_back(entry.path().string());
			g_lib_lists.back().load();
		}
	} catch(...) {}

	auto args_hash =
		args_parser("GTS " GTS_VERSION_STR "\n"
					"Description of command line parameters:")

			.add_flag ("start, -start, --start"      , "Start the server."                                                               , GC_SA_START  )
			.add_flag ("stop, -stop, --stop"         , "Stop the server."                                                                , GC_SA_STOP   )
			.add_flag ("restart, -restart, --restart", "Restart the server."                                                             , GC_SA_RESTART)
			.add_flag ("-d, --daemon"                , "Start as a daemon process."                                                      , GC_SA_DAEMON )
			.add_group("-i, --instace"               , "Specify the server instance name to start multiple instances. (default is 'gts')", GC_SA_INSNAME)
			.add_group("-f, --file"                  , "Specify the configuration file (default is './config.ini')."                     , GC_SA_CFPATH )
			.add_flag ("status, -status, --status"   , "Viewing server status."                                                          , GC_SA_STATUS )
#if 0
			.add_flag ("--view-subserver-all"        , "Viewing all subservers."                                                         , GC_SA_VASUS  )
			.add_flag ("--view-subserver"            , "Viewing running subservers."                                                     , GC_SA_VRSUS  )
			.add_flag ("--start-subserver-all"       , "Start all subservers."                                                           , GC_SA_STSSA  )
			.add_group("--start-subserver"           , "Start subservices in the list."                                                  , GC_SA_STSS   )
			.add_flag ("--stop-subserver-all"        , "Stop all subservers."                                                            , GC_SA_SPSSA  )
			.add_group("--stop-subserver"            , "Stop subservices in the list."                                                   , GC_SA_SPSS   )
#endif
			.enable_h()
			.set_help_extension(plugin_call_handle::extension::view_help())
			.set_version(view_version(false))
			.set_v(view_version(true))

			.parsing(argc, argv, others);

	if( not others.empty() and plugin_call_handle::extension::args_parsing(others) == false )
	{
		std::cerr << "Invalid arguments." << std::endl;
		exit(-1);
	}
	return args_hash;
}

/*----------------------------------------------------------------------------------------------------------------------*/

#ifdef __llvm__
# define IS_CLANG  "Clang-"
#else //no llvm
# define IS_CLANG  ""
#endif //Clang

static std::string view_version(bool all)
{
	std::string result;
	result += "GTS " GTS_VERSION_STR;
	if( not all )
		return result;

	result += "\n";
#ifdef GTS_ENABLE_SSL
	result += OPENSSL_VERSION_TEXT "\n";
#endif //ssl

#ifdef __GNUC__
	result += "Compiler: " IS_CLANG "GCC " __VERSION__ "\n";
#elif defined(_MSC_VER)
	result += "Compiler: " IS_CLANG "MSVC " _MSC_VER "\n";
#endif //GCC

	result += "Compilation time: " __DATE__ " " __TIME__;
	auto ext_info = plugin_call_handle::extension::view_version();

	if( not ext_info.empty() )
		result += "\n" + ext_info;
	return result;
}

}} //namespace gts::cmdline