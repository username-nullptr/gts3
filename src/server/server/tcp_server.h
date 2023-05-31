#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <thread>
#include <rttr/type>
#include "gts/socket.h"
#include "gts/log.h"

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
	std::string view_status() const;

private:
	template <class asio_socket>
	void service(std::shared_ptr<socket<asio_socket>> _socket, bool ipv6);
	void new_connect_method_init();

private:
	class basic_site
	{
		DISABLE_COPY(basic_site)

	public:
		basic_site(tcp_server *q_ptr, asio::io_context &io, const std::string &addr, uint16_t port);
		virtual ~basic_site() = 0;

	public:
		virtual std::string view_status() const;
		virtual bool start();
		void stop();

	protected:
		tcp_server *q_ptr;
		tcp::acceptor m_acceptor;
		tcp::endpoint m_endpoint;
		std::string m_addr;
		bool m_ipv6;
	};

	class tcp_site : public basic_site
	{
	public:
		using basic_site::basic_site;
		std::string view_status() const override;
		bool start() override;

	private:
		void do_accept();
	};

#ifdef GTS_ENABLE_SSL
	class ssl_site : public basic_site
	{
	public:
		using basic_site::basic_site;
		std::string view_status() const override;
		bool start() override;

	private:
		void do_accept();
	};
#endif //ssl

private:
	typedef std::shared_ptr<basic_site>  site_ptr;
	std::map<std::string, site_ptr> m_sites;

private:
	std::shared_ptr<rttr::library> m_plugin_lib;
	rttr::method m_new_connect_method;

	int m_ncma_count = 1;
	int m_buffer_size = 65536;
};

template <class asio_socket>
void tcp_server::service(std::shared_ptr<socket<asio_socket>> _socket, bool ipv6)
{
	asio::error_code error;
	_socket->set_option(tcp::socket::send_buffer_size(m_buffer_size), error);
	if( error )
		log_error("asio: set socket send buffer error: {}. ({})\n", error.message(), error.value());

	_socket->set_option(tcp::socket::receive_buffer_size(m_buffer_size), error);
	if( error )
		log_error("asio: set socket receive buffer error: {}. ({})\n", error.message(), error.value());

	else if( m_ncma_count == 3 )
		m_new_connect_method.invoke({}, _socket->release(), reinterpret_cast<void*>(_socket->release_ssl()), ipv6);

	else if( m_ncma_count == 2 )
		m_new_connect_method.invoke({}, _socket->release(), reinterpret_cast<void*>(_socket->release_ssl()));
}

} //namespace gts


#endif //TCP_SERVER_H
