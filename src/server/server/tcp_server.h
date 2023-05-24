#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <thread>
#include <rttr/type>
#include "gts_log.h"
#include "socket.h"

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
	void start_tcp();

#ifdef GTS_ENABLE_SSL
	void start_ssl();
#endif //ssl

public:
	template <class asio_socket>
	void service(std::shared_ptr<socket<asio_socket>> _socket);

private:
	tcp::acceptor m_acceptor;
	uint16_t m_port;

	std::shared_ptr<rttr::library> m_plugin_lib;
	rttr::method m_new_connect_method;
	int m_ncma_count = 1;
	int m_protocol = 4;

	int m_buffer_size = 65536;
};

template <class asio_socket>
void tcp_server::service(std::shared_ptr<socket<asio_socket>> _socket)
{
	asio::error_code error;
	_socket->set_option(tcp::socket::send_buffer_size(m_buffer_size), error);
	if( error )
		log_error("asio: set socket send buffer error: {}. ({})\n", error.message(), error.value());

	_socket->set_option(tcp::socket::receive_buffer_size(m_buffer_size), error);
	if( error )
		log_error("asio: set socket receive buffer error: {}. ({})\n", error.message(), error.value());

	else if( m_ncma_count == 3 )
		m_new_connect_method.invoke({}, _socket->release(), socket<asio_socket>::is_ssl::value, m_protocol);

	else if( m_ncma_count == 2 )
		m_new_connect_method.invoke({}, _socket->release(), socket<asio_socket>::is_ssl::value);
}

} //namespace gts


#endif //TCP_SERVER_H
