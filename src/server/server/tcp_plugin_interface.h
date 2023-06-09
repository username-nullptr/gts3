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
 * new_connect(asio::ip::tcp::socket &sock, void *ssl)
 * new_connect(socket hanlde, void *ssl, bool is_ipv6)
 * new_connect(socket handle, void *ssl)
*/
#define GTS_PLUGIN_INTERFACE_NEW_CONNECT  GTS_PLUGIN_INTERFACE "new_connection"

/*
 * std::string view_status(void)
*/
#define GTS_PLUGIN_INTERFACE_VIEW_STATUS  GTS_PLUGIN_INTERFACE "view_status"


#endif //GTS_TCP_PLUGIN_INTERFACE_H
