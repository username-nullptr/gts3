#ifndef SERVER_TOOL_H
#define SERVER_TOOL_H

#include "gts/tcp_socket.h"
#include "gts/string_list.h"

namespace gts
{

GTSCORE_API site_info_map &server_get_site_infos();

namespace plugin_call
{

void init(const std::string &config_file);
void exit();

bool new_connection(tcp_socket *sock, bool universal);
std::string view_status();

} //namespace plugin_call

namespace extension { namespace plugin_call
{

void init(const std::string &config_file);
void exit();

bool args_parsing(const string_list &args);
std::string view_version();
std::string view_help();

}} //namespace extension::plugin_call

} //namespace gts


#endif //SERVER_TOOL_H
