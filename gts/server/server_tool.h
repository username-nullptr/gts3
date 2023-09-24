#ifndef SERVER_TOOL_H
#define SERVER_TOOL_H

#include "gts/tcp_socket.h"
#include "gts/string_list.h"

namespace gts
{

GTSCORE_API site_info_map &server_get_site_infos();

class plugin_call_handle
{
public:
	static void init(const std::string &json_file, const std::string &config_file);
	static void exit();

public:
	static bool new_connection(tcp_socket *sock, bool universal);
	static std::string view_status();

public:
	class extension
	{
	public:
		static void init(const std::string &config_file);
		static void exit();

	public:
		static bool args_parsing(const string_list &args);
		static std::string view_version();
		static std::string view_help();
	};
};

} //namespace gts


#endif //SERVER_TOOL_H
