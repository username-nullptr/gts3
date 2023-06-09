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

DECL_EXPORT void args_parsing(int argc, const char *argv[])
{
	for(int i=0; i<argc; i++)
		std::cerr << "argv[" << i << "] = " << argv[i] << std::endl;
}

DECL_EXPORT void extern_init()
{
	std::cerr << "extern init ..." << std::endl;
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
			.method("gts.cmdline.plugin.init", extern_init)
			.method("gts.cmdline.plugin.version", view_version_info);
}
