
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include "arguments.h"
#include "server_tool.h"

#include <rttr/library.h>
#include <cppfilesystem>
#include <iostream>

#ifdef GTS_ENABLE_SSL
# include <openssl/ssl.h>
#endif //ssl

GTS_CMDLINE_NAMESPACE_BEGIN

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

GTS_CMDLINE_NAMESPACE_END
