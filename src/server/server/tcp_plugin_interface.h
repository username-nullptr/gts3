#ifndef GTS_TCP_PLUGIN_INTERFACE_H
#define GTS_TCP_PLUGIN_INTERFACE_H

#include <rttr/registration>

#define GTS_PLUGIN_INTERFACE  "gts.plugin."

/*
 * init(void)
 * init(const std::string &config_file)
*/
#define GTS_PLUGIN_INTERFACE_INIT  GTS_PLUGIN_INTERFACE "init"

/*
 * exit(void)
*/
#define GTS_PLUGIN_INTERFACE_EXIT  GTS_PLUGIN_INTERFACE "exit"

/*
 * new_connect(socket handle/descriptor)
 * new_connect(socket handle/descriptor, int 4/6)
*/
#define GTS_PLUGIN_INTERFACE_NEW_CONNECT  GTS_PLUGIN_INTERFACE "new_connect"

/*
 * std::string view_status(void)
*/
#define GTS_PLUGIN_INTERFACE_VIEW_STATUS  GTS_PLUGIN_INTERFACE "view_status"


#endif //GTS_TCP_PLUGIN_INTERFACE_H
