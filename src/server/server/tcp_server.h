#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <thread>
#include <asio.hpp>
#include <rttr/type>
#include "gts/gts_global.h"

using namespace asio::ip;

namespace gts
{

class tcp_server
{
	DISABLE_COPY(tcp_server)

public:
	tcp_server(int argc, const char *argv[]);
	~tcp_server();

public:
	void start();
	void stop();

public:
	uint16_t port() const;
	std::string view_status() const;

private:
	tcp::acceptor m_acceptor;
	uint16_t m_port;

	std::shared_ptr<rttr::library> m_plugin_lib;
	rttr::method m_new_connect_method;
	int m_ncma_count = 1;
	int m_protocol = 4;

	int m_buffer_size = 65536;
};

} //namespace gts


#endif //TCP_SERVER_H
