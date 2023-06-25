#include "ssl_socket.h"

namespace gts
{

ssl_socket::ssl_socket(ssl_stream *sock) :
	tcp_socket(&sock->next_layer()),
	m_ssl_sock(sock)
{

}

ssl_socket::~ssl_socket()
{
	m_sock = nullptr;
}

std::size_t ssl_socket::write_some(const std::string &buf, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_ssl_sock->write_some(asio::buffer(buf), error);
}

std::size_t ssl_socket::write_some(const void *buf, std::size_t size, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_ssl_sock->write_some(asio::buffer(buf, size), error);
}

std::size_t ssl_socket::read_some(std::string &buf, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_ssl_sock->read_some(asio::buffer(buf), error);
}

std::size_t ssl_socket::read_some(void *buf, std::size_t size, asio::error_code &error)
{
	m_sock->non_blocking(false);
	return m_ssl_sock->read_some(asio::buffer(buf, size), error);
}

void ssl_socket::async_write_some(const std::string &buf, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_ssl_sock->async_write_some(asio::buffer(buf), callback);
}

void ssl_socket::async_write_some(const void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_ssl_sock->async_write_some(asio::buffer(buf, size), callback);
}

void ssl_socket::async_read_some(std::string &buf, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_ssl_sock->async_read_some(asio::buffer(buf), callback);
}

void ssl_socket::async_read_some(void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)> callback)
{
	m_ssl_sock->async_read_some(asio::buffer(buf, size), callback);
}

ssl::context &ssl_socket::asio_ssl_context()
{
	static ssl::context ssl_context { ssl::context::sslv23_server };
	return ssl_context;
}

} //namespace gts
