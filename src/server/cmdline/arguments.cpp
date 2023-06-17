#include "arguments.h"
#include "startup_plugin_interface.h"

#include <rttr/library.h>
#include <iostream>

#if GTS_ENABLE_SSL
# include <openssl/ssl.h>
#endif //ssl

namespace gts { namespace cmdline
{

static void args_parsing_extensions(const string_list &args);
static std::string view_version(bool all = false);
static std::string view_help_extension();

// Modify or extend this function
args_parser::arguments argument_check(int argc, const char *argv[], string_list &others)
{
	auto args_hash =
		args_parser("GTS " GTS_VERSION_STR "\n"
					"Description of command line parameters:")

			.add_flag ("start, -start, --start"      , "Start the server."                                                               , GC_SA_START)
			.add_flag ("stop, -stop, --stop"         , "Stop the server."                                                                , GC_SA_STOP)
			.add_flag ("restart, -restart, --restart", "Restart the server."                                                             , GC_SA_RESTART)
			.add_flag ("-d, --daemon"                , "Start as a daemon process."                                                      , GC_SA_DAEMON)
			.add_group("-i, --instace"               , "Specify the server instance name to start multiple instances. (default is 'gts')", GC_SA_INSNAME)
			.add_group("-f, --file"                  , "Specify the configuration file (default is './config.ini')."                     , GC_SA_CFPATH)
			.add_flag ("status, -status, --status"   , "Viewing server status."                                                          , GC_SA_STATUS)
			.add_flag ("--view-subserver-all"        , "Viewing all subservers."                                                         , GC_SA_VASUS)
			.add_flag ("--view-subserver"            , "Viewing running subservers."                                                     , GC_SA_VRSUS)
			.add_flag ("--start-subserver-all"       , "Start all subservers."                                                           , GC_SA_STSSA)
			.add_group("--start-subserver"           , "Start subservices in the list."                                                  , GC_SA_STSS)
			.add_flag ("--stop-subserver-all"        , "Stop all subservers."                                                            , GC_SA_SPSSA)
			.add_group("--stop-subserver"            , "Stop subservices in the list."                                                   , GC_SA_SPSS)

			.enable_h()
			.set_help_extension(view_help_extension())
			.set_version(view_version(false))
			.set_v(view_version(true))

			.parsing(argc, argv, others);

	args_parsing_extensions(others);
	return args_hash;
}

/*----------------------------------------------------------------------------------------------------------------------*/

static inline bool load_startup_extensions_library()
{
	static rttr::library library("gtsstartupextensions");
	return library.load();
}

static void args_parsing_extensions(const string_list &args)
{
	if( args.empty() )
		return ;

	bool res = false;
	if( load_startup_extensions_library() )
	{
		auto method = rttr::type::get_global_method(GTS_STARTUP_PLUGIN_INTERFACE_ARGS_PARSING,
													{rttr::type::get<int>(), rttr::type::get<const char**>()});
		if( method.is_valid() )
		{
			if( method.get_return_type() == rttr::type::get<bool>() )
			{
				static auto other_args_vector = args.c_str_vector();
				res = method.invoke({}, static_cast<int>(other_args_vector.size()), other_args_vector.data()).to_bool();
			}
		}
		else
		{
			method = rttr::type::get_global_method(GTS_STARTUP_PLUGIN_INTERFACE_ARGS_PARSING,
												   {rttr::type::get<std::deque<std::string>>()});

			if( method.is_valid() and method.get_return_type() == rttr::type::get<bool>() )
				res = method.invoke({}, static_cast<std::deque<std::string>>(args)).to_bool();
		}
	}
	if( not res )
	{
		std::cerr << "Invalid arguments." << std::endl;
		exit(-1);
	}
}

static std::string view_extension(const char *method_name);

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
#if GTS_ENABLE_SSL
	result += OPENSSL_VERSION_TEXT "\n";
#endif //ssl

#ifdef __GNUC__
	result += "Compiler: " IS_CLANG "GCC " __VERSION__ "\n";
#elif defined(_MSC_VER)
	result += "Compiler: " IS_CLANG "MSVC " _MSC_VER "\n";
#endif //GCC

	result += "Compilation time: " __DATE__ " " __TIME__ "\n" +
			  view_extension(GTS_STARTUP_PLUGIN_INTERFACE_VERSION);
	return result;
}

static inline std::string view_help_extension()
{
	return view_extension(GTS_STARTUP_PLUGIN_INTERFACE_HELP_EX);
}

static std::string view_extension(const char *method_name)
{
	std::string result;
	if( load_startup_extensions_library() == false )
		return result;
	auto method = rttr::type::get_global_method(method_name);

	if( method.is_valid() and method.get_parameter_infos().size() == 0 and
		method.get_return_type() == rttr::type::get<std::string>() )
	{
		auto str = method.invoke({}).to_string();
		if( str.empty() )
			return result;

		int i = str.size() - 1;
		for(; i>=0; i--)
		{
			if( str[i] != '\n' )
				break;
		}
		str = str.substr(0, i + 1);

		if( not str.empty() )
			result += "\nExtern:\n" + str;
	}
	return result;
}

}} //namespace gts::cmdline
