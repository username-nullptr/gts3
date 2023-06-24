#include <gts/registration.h>
#include <iostream>

namespace gts { namespace cmdline { namespace business
{

static void extensions_init()
{
	std::cerr << "extern init ..." << std::endl;
}

static void extensions_exit()
{
	std::cerr << "extern exit ..." << std::endl;
}

static bool args_parsing(int argc, const char *argv[])
{
	for(int i=0; i<argc; i++)
		std::cerr << "argv[" << i << "] = " << argv[i] << std::endl;
	return true;
}

static std::string view_version_info()
{
	return "gts cmdline extern examples. v0.0.0\n";
}

static std::string view_help()
{
	return "    gts cmdline extern examples: help.\n";
}

}}} //gts::cmdline::business

RTTR_PLUGIN_REGISTRATION
{
	using namespace gts::cmdline::business;

	gts::extension::registration()
			.init_method(extensions_init)
			.exit_method(extensions_exit)
			.args_parsing_method(args_parsing)
			.view_version_method(view_version_info)
			.view_help_method(view_help);
}
