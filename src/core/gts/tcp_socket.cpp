#include "tcp_socket.h"
#include "gts/fmt_formatter.h"

#include <fmt/format.h>
#include <iostream>

namespace gts
{

tcp_socket::tcp_socket(tcp::socket *sock) :
	m_sock(sock)
{
	assert(sock);
}

tcp_socket::~tcp_socket()
{
	if( m_sock )
		delete m_sock;
}

std::size_t tcp_socket::write_some(const std::string &buf, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_sock->write_some(asio::buffer(buf), error);
}

std::size_t tcp_socket::write_some(const void *buf, std::size_t size, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_sock->write_some(asio::buffer(buf, size), error);
}

std::size_t tcp_socket::read_some(std::string &buf, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_sock->read_some(asio::buffer(buf), error);
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_sock->read_some(asio::buffer(buf, size), error);
}

void tcp_socket::async_write_some(const std::string &buf, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_sock->async_write_some(asio::buffer(buf), callback);
}

void tcp_socket::async_write_some(const void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_sock->async_write_some(asio::buffer(buf, size), callback);
}

void tcp_socket::async_read_some(std::string &buf, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_sock->async_read_some(asio::buffer(buf), callback);
}

void tcp_socket::async_read_some(void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_sock->async_read_some(asio::buffer(buf, size), callback);
}

std::size_t tcp_socket::write_some(const std::string &buf)
{
	asio::error_code error;
	auto res = write_some(buf, error);
	if( error )
		std::cerr << fmt::format("*** Error: tcp_socket::write_some(std::string) error: {}", error) << std::endl;
	return res;
}

std::size_t tcp_socket::write_some(const void *buf, std::size_t size)
{
	asio::error_code error;
	auto res = write_some(buf, size, error);
	if( error )
		std::cerr << fmt::format("*** Error: tcp_socket::write_some(void*) error: {}", error) << std::endl;
	return res;
}

std::size_t tcp_socket::read_some(std::string &buf)
{
	asio::error_code error;
	auto res = read_some(buf, error);
	if( error )
		std::cerr << fmt::format("*** Error: tcp_socket::read_some(std::string) error: {}", error) << std::endl;
	return res;
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size)
{
	asio::error_code error;
	auto res = read_some(buf, size, error);
	if( error )
		std::cerr << fmt::format("*** Error: tcp_socket::read_some(void*) error: {}", error) << std::endl;
	return res;
}

void tcp_socket::async_write_some(const std::string &buf, std::function<void(std::size_t)> callback)
{
	async_write_some(buf, [callback](const asio::error_code &error, std::size_t size)
	{
		if( error )
			std::cerr << fmt::format("*** Error: tcp_socket::async_write_some(std::string) error: {}", error) << std::endl;
		callback(size);
	});
}

void tcp_socket::async_write_some(const void *buf, std::size_t size, std::function<void(std::size_t)> callback)
{
	async_write_some(buf, size, [callback](const asio::error_code &error, std::size_t size)
	{
		if( error )
			std::cerr << fmt::format("*** Error: tcp_socket::async_write_some(void*) error: {}", error) << std::endl;
		callback(size);
	});
}

void tcp_socket::async_read_some(std::string &buf, std::function<void(std::size_t)> callback)
{
	async_read_some(buf, [callback](const asio::error_code &error, std::size_t size)
	{
		if( error )
			std::cerr << fmt::format("*** Error: tcp_socket::async_read_some(std::string) error: {}", error) << std::endl;
		callback(size);
	});
}

void tcp_socket::async_read_some(void *buf, std::size_t size, std::function<void(std::size_t)> callback)
{
	async_read_some(buf, size, [callback](const asio::error_code &error, std::size_t size)
	{
		if( error )
			std::cerr << fmt::format("*** Error: tcp_socket::async_read_some(void*) error: {}", error) << std::endl;
		callback(size);
	});
}

tcp::endpoint tcp_socket::remote_endpoint(asio::error_code &error)
{
	return m_sock->remote_endpoint(error);
}

tcp::endpoint tcp_socket::remote_endpoint()
{
	return m_sock->remote_endpoint();
}

tcp::endpoint tcp_socket::local_endpoint(asio::error_code &error)
{
	return m_sock->local_endpoint(error);
}

tcp::endpoint tcp_socket::local_endpoint()
{
	return m_sock->local_endpoint();
}

void tcp_socket::shutdown(asio::error_code &error, tcp::socket::shutdown_type what)
{
	m_sock->shutdown(what, error);
}

void tcp_socket::shutdown(tcp::socket::shutdown_type what)
{
	m_sock->shutdown(what);
}

bool tcp_socket::is_open() const
{
	return m_sock->is_open();
}

void tcp_socket::close(asio::error_code &error, bool _shutdown)
{
	if( _shutdown )
		shutdown(error, tcp::socket::shutdown_both);
	m_sock->close(error);
}

void tcp_socket::close(bool _shutdown)
{
	if( _shutdown )
		shutdown(tcp::socket::shutdown_both);
	m_sock->close();
}

void tcp_socket::non_blocking(bool mode, asio::error_code &error)
{
	m_sock->non_blocking(mode, error);
}

void tcp_socket::non_blocking(bool mode)
{
	m_sock->non_blocking(mode);
}

bool tcp_socket::non_blocking() const
{
	return m_sock->non_blocking();
}

const tcp::socket &tcp_socket::native() const
{
	return *m_sock;
}

tcp::socket &tcp_socket::native()
{
	return *m_sock;
}

} //namespace gts
