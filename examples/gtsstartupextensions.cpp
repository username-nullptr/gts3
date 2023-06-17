#include <rttr/registration>
#include <iostream>

#ifdef _MSC_VER
# define DECL_EXPORT  __declspec(dllexport)
#elif defined(__GNUC__)
# define DECL_EXPORT  __attribute__((visibility("default")))
#else // other compiler
# define DECL_EXPORT
#endif //compiler

namespace gts { namespace cmdline { namespace business
{

DECL_EXPORT bool args_parsing(int argc, const char *argv[])
{
	for(int i=0; i<argc; i++)
		std::cerr << "argv[" << i << "] = " << argv[i] << std::endl;
	return true;
}

DECL_EXPORT void extensions_init()
{
	std::cerr << "extern init ..." << std::endl;
}

DECL_EXPORT void extensions_exit()
{
	std::cerr << "extern exit ..." << std::endl;
}

DECL_EXPORT std::string view_version_info()
{
	return "gts cmdline extern examples. v0.0.0\n";
}

}}} //gts::cmdline::business

RTTR_PLUGIN_REGISTRATION
{
	using namespace gts::cmdline::business;

	rttr::registration::
			method("gts.cmdline.plugin.args_parsing", args_parsing)
			.method("gts.cmdline.plugin.init", extensions_init)
			.method("gts.cmdline.plugin.exit", extensions_exit)
			.method("gts.cmdline.plugin.version", view_version_info);
}
