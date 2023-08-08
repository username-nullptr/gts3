#ifndef GTS_WEBSOCKET_H
#define GTS_WEBSOCKET_H

#include <gts/web_global.h>
#include <gts/tcp_socket.h>

#ifdef GTS_ENABLE_SSL
# include <gts/ssl_socket.h>
#endif //ssl

// Not Implemented
namespace gts
{

class GTSWEB_API websocket
{
	GTS_DISABLE_COPY_MOVE(websocket)
	using duration = std::chrono::milliseconds;

public:
	explicit websocket(tcp_socket_ptr sock);
	virtual ~websocket();

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
	void write_some_nonblock(const std::string &buf);
	void write_some_nonblock(const void *buf, std::size_t size);

public:
	std::size_t read_some(std::string &buf, const duration &timeout, asio::error_code &error);
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout, asio::error_code &error);
	std::size_t read_some(std::string &buf, const duration &timeout);
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout);

public:
	bool wait_writeable(const duration &ms, asio::error_code &error);
	bool wait_readable(const duration &ms, asio::error_code &error);
	bool wait_writeable(const duration &ms);
	bool wait_readable(const duration &ms);

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
	tcp_socket_ptr m_sock;
};

} //namespace gts


#endif //GTS_WEBSOCKET_H