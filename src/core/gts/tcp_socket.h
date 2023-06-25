#ifndef GTS_TCP_SOCKET_H
#define GTS_TCP_SOCKET_H

#include <gts/gts_global.h>
#include <functional>

using namespace asio::ip;

namespace gts
{

class GTSCORE_API tcp_socket
{
	GTS_DISABLE_COPY_MOVE(tcp_socket)

public:
	explicit tcp_socket(tcp::socket *sock);
	virtual ~tcp_socket();

public:
	virtual std::size_t write_some(const std::string &buf, asio::error_code &error);
	virtual std::size_t write_some(const void *buf, std::size_t size, asio::error_code &error);
	virtual std::size_t read_some(std::string &buf, asio::error_code &error);
	virtual std::size_t read_some(void *buf, std::size_t size, asio::error_code &error);

public:
	virtual void async_write_some(const std::string &buf, std::function<void(asio::error_code, std::size_t)>);
	virtual void async_write_some(const void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>);
	virtual void async_read_some(std::string &buf, std::function<void(asio::error_code, std::size_t)>);
	virtual void async_read_some(void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>);

public:
	std::size_t write_some(const std::string &buf);
	std::size_t write_some(const void *buf, std::size_t size);
	std::size_t read_some(std::string &buf);
	std::size_t read_some(void *buf, std::size_t size);

public:
	void async_write_some(const std::string &buf, std::function<void(std::size_t)>);
	void async_write_some(const void *buf, std::size_t size, std::function<void(std::size_t)>);
	void async_read_some(std::string &buf, std::function<void(std::size_t)>);
	void async_read_some(void *buf, std::size_t size, std::function<void(std::size_t)>);

public:
	tcp::endpoint remote_endpoint(asio::error_code &error);
	tcp::endpoint remote_endpoint();

public:
	tcp::endpoint local_endpoint(asio::error_code &error);
	tcp::endpoint local_endpoint();

public:
	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption& option, asio::error_code &error);

	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption& option);

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption& option, asio::error_code &error) const;

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption& option) const;

public:
	void shutdown(asio::error_code &error, tcp::socket::shutdown_type what = tcp::socket::shutdown_both);
	void shutdown(tcp::socket::shutdown_type what = tcp::socket::shutdown_both);

public:
	bool is_open() const;
	void close(asio::error_code &error, bool shutdown = false);
	void close(bool shutdown = false);

public:
	void non_blocking(bool mode, asio::error_code& error);
	void non_blocking(bool mode);
	bool non_blocking() const;

public:
	const tcp::socket &native() const;
	tcp::socket &native();

protected:
	tcp::socket *m_sock;
};

typedef std::shared_ptr<tcp_socket>  tcp_socket_ptr;

template <typename SettableSocketOption>
void tcp_socket::set_option(const SettableSocketOption& option, asio::error_code &error) {
	m_sock->set_option(option, error);
}

template <typename SettableSocketOption>
void tcp_socket::set_option(const SettableSocketOption& option) {
	m_sock->set_option(option);
}

template <typename GettableSocketOption>
void tcp_socket::get_option(GettableSocketOption& option, asio::error_code &error) const {
	m_sock->get_option(option, error);
}

template <typename GettableSocketOption>
void tcp_socket::get_option(GettableSocketOption& option) const {
	m_sock->get_option(option);
}

} //namespace gts


#endif //GTS_TCP_SOCKET_H