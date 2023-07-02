#include <gts/registration.h>
#include <iostream>

namespace gts { namespace cmdline { namespace business
{

GTS_DECL_EXPORT void extensions_init()
{
	std::cerr << "extern init ..." << std::endl;
}

GTS_DECL_EXPORT void extensions_exit()
{
	std::cerr << "extern exit ..." << std::endl;
}

GTS_DECL_EXPORT bool args_parsing(int argc, const char *argv[])
{
	for(int i=0; i<argc; i++)
		std::cerr << "argv[" << i << "] = " << argv[i] << std::endl;
	return true;
}

GTS_DECL_EXPORT std::string view_version_info()
{
	return "gts cmdline extern examples. v0.0.0\n";
}

GTS_DECL_EXPORT std::string view_help()
{
	return "    gts cmdline extern examples: help.\n";
}

}}} //gts::cmdline::business

GTS_PLUGIN_REGISTRATION
{
	using namespace gts::cmdline::business;

	gts::extension::registration()
			.init_method(extensions_init)
			.exit_method(extensions_exit)
			.args_parsing_method(args_parsing)
			.view_version_method(view_version_info)
			.view_help_method(view_help);
}
