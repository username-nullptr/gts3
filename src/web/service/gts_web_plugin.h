#ifndef GTS_WEB_PLUGIN_H
#define GTS_WEB_PLUGIN_H

#include <rttr/registration>

#define GTS_WEB_PLUGIN_INTERFACE  "gts.web.plugin."

/*
 * init(void)
 * init(const std::string &config_file)
*/
#define GTS_WEB_PLUGIN_INTERFACE_INIT  ".init"

/*
 * exit(void)
*/
#define GTS_WEB_PLUGIN_INTERFACE_EXIT  ".exit"

/*
 * plugin_class::set_version(const std::string &version)
*/
#define GTS_WEB_PLUGIN_INTERFACE_SET_VERSION  "set_version"

/*
 * plugin_class::set_method(const std::string &method)
*/
#define GTS_WEB_PLUGIN_INTERFACE_SET_METHOD  "set_method"

/*
 * plugin_class::set_path(const std::string &path)
*/
#define GTS_WEB_PLUGIN_INTERFACE_SET_PATH  "set_path"

/*
 * plugin_class::add_parameter(const std::string &key, const std::string &value)
*/
#define GTS_WEB_PLUGIN_INTERFACE_ADD_PARAMETER  "add_parameter"

/*
 * plugin_class::add_header(const std::string &key, const std::string &value)
*/
#define GTS_WEB_PLUGIN_INTERFACE_ADD_HEADER  "add_header"

/*
 * plugin_class::add_env(const std::string &key, const std::string &value)
*/
#define GTS_WEB_PLUGIN_INTERFACE_ADD_ENV  "add_env"

/*
 * plugin_class::set_body(const std::string &body)
*/
#define GTS_WEB_PLUGIN_INTERFACE_SET_BODY  "set_body"

/*
 * new_request(socket handle/descriptor)
 * new_request(socket handle/descriptor, int 4/6)
*/
#define GTS_WEB_PLUGIN_INTERFACE_CALL  "call"


#endif //GTS_WEB_PLUGIN_H
