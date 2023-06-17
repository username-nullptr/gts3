#ifndef GTS_STARTUP_EXTERN_PLUGIN_INTERFACE_H
#define GTS_STARTUP_EXTERN_PLUGIN_INTERFACE_H

#include <rttr/registration>

#define GTS_STARTUP_PLUGIN_INTERFACE  "gts.cmdline.plugin."

/*
 * bool args_parsing(int argc, const char *argv[])
 * bool args_parsing(const std::deque<std::string> &args)
*/
#define GTS_STARTUP_PLUGIN_INTERFACE_ARGS_PARSING  GTS_STARTUP_PLUGIN_INTERFACE "args_parsing"

/*
 * std::string view_version_info()
*/
#define GTS_STARTUP_PLUGIN_INTERFACE_VERSION  GTS_STARTUP_PLUGIN_INTERFACE "version"

/*
 * std::string view_help_extension()
*/
#define GTS_STARTUP_PLUGIN_INTERFACE_HELP_EX  GTS_STARTUP_PLUGIN_INTERFACE "help_extension"

/*
 * init(void)
 * init(const std::string &config_file)
*/
#define GTS_STARTUP_PLUGIN_INTERFACE_INIT  GTS_STARTUP_PLUGIN_INTERFACE "init"

/*
 * exit(void)
*/
#define GTS_STARTUP_PLUGIN_INTERFACE_EXIT  GTS_STARTUP_PLUGIN_INTERFACE "exit"


#endif //GTS_STARTUP_EXTERN_PLUGIN_INTERFACE_H
