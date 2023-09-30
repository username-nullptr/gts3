#ifndef GTS_WEB_SOCKET_H
#define GTS_WEB_SOCKET_H

#include <gts/web/types.h>
#include <gts/tcp_socket.h>

#ifdef GTS_ENABLE_SSL
# include <gts/ssl_socket.h>
#endif //ssl

// Not Implemented
namespace gts { namespace web
{

class GTS_WEB_API socket
{
	GTS_DISABLE_COPY_MOVE(socket)
	using duration = std::chrono::milliseconds;

public:
	explicit socket(tcp_socket_ptr sock);
	virtual ~socket() noexcept;

public:
	std::size_t write_some(const std::string &buf, asio::error_code &error) noexcept;
	std::size_t write_some(const void *buf, std::size_t size, asio::error_code &error) noexcept;
	std::size_t read_some(std::string &buf, asio::error_code &error) noexcept;
	std::size_t read_some(void *buf, std::size_t size, asio::error_code &error) noexcept;

public:
	void async_write_some(const std::string &buf, std::function<void(asio::error_code, std::size_t)>) noexcept;
	void async_write_some(const void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>) noexcept;
	void async_read_some(std::string &buf, std::function<void(asio::error_code)>) noexcept;
	void async_read_some(std::string &buf, std::size_t, std::function<void(asio::error_code)>) noexcept;
	void async_read_some(void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>) noexcept;

public:
	std::size_t write_some(const std::string &buf) noexcept;
	std::size_t write_some(const void *buf, std::size_t size) noexcept;
	std::size_t read_some(std::string &buf) noexcept;
	std::size_t read_some(void *buf, std::size_t size) noexcept;

public:
	void async_write_some(const std::string &buf, std::function<void(std::size_t)>) noexcept;
	void async_write_some(const void *buf, std::size_t size, std::function<void(std::size_t)>) noexcept;
	void async_read_some(std::string &buf, std::function<void()>) noexcept;
	void async_read_some(std::string &buf, std::size_t, std::function<void()>) noexcept;
	void async_read_some(void *buf, std::size_t size, std::function<void(std::size_t)>) noexcept;

public:
	void write_some_nonblock(std::string buf) noexcept;
	void write_some_nonblock(const void *buf, std::size_t size) noexcept;

public:
	std::size_t read_some(std::string &buf, const duration &timeout, asio::error_code &error) noexcept;
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout, asio::error_code &error) noexcept;
	std::size_t read_some(std::string &buf, const duration &timeout) noexcept;
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout) noexcept;

public:
	bool wait_writeable(const duration &ms, asio::error_code &error) noexcept;
	bool wait_readable(const duration &ms, asio::error_code &error) noexcept;
	bool wait_writeable(const duration &ms) noexcept;
	bool wait_readable(const duration &ms) noexcept;

public:
	tcp::endpoint remote_endpoint(asio::error_code &error) noexcept;
	tcp::endpoint remote_endpoint() noexcept;

public:
	tcp::endpoint local_endpoint(asio::error_code &error) noexcept;
	tcp::endpoint local_endpoint() noexcept;

public:
	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption &option, asio::error_code &error) noexcept;

	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption &option) noexcept;

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption &option, asio::error_code &error) const noexcept;

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption &option) const noexcept;

public:
	void shutdown(asio::error_code &error, tcp::socket::shutdown_type what = tcp::socket::shutdown_both) noexcept;
	void shutdown(tcp::socket::shutdown_type what = tcp::socket::shutdown_both) noexcept;

public:
	bool is_open() const;
	void close(asio::error_code &error, bool shutdown = false) noexcept;
	void close(bool shutdown = false) noexcept;

public:
	void non_blocking(bool mode, asio::error_code& error) noexcept;
	void non_blocking(bool mode) noexcept;
	bool non_blocking() const noexcept;

public:
	const tcp_socket &native() const noexcept;
	tcp_socket &native() noexcept;

protected:
	tcp_socket_ptr m_sock;
};

using socket_ptr = std::shared_ptr<socket>;

} //namespace web

using websocket = web::socket;

using websocket_ptr = web::socket_ptr;

} //namespace gts


#endif //GTS_WEB_SOCKET_H
