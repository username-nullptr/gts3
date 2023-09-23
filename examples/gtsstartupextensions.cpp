#include <gts/registration.h>
#include <iostream>

namespace gts { namespace cmdline { namespace business
{

GTS_PLUGIN_CMD_INIT()
{
	std::cerr << "extern init ..." << std::endl;
}

GTS_PLUGIN_CMD_EXIT()
{
	std::cerr << "extern exit ..." << std::endl;
}

GTS_PLUGIN_CMD_ARGS_PARSING(int argc, const char *argv[])
{
	for(int i=0; i<argc; i++)
		std::cerr << "argv[" << i << "] = " << argv[i] << std::endl;
	return true;
}

GTS_PLUGIN_CMD_VIEW_VERSION()
{
	return "Extern examples: v0.0.0\n";
}

GTS_PLUGIN_CMD_VIEW_HELP()
{
	return "    -- extern examples\n";
}

}}} //gts::cmdline::business
