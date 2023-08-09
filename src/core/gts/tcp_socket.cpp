#include "tcp_socket.h"
#include "gts/formatter.h"

#include <cppformat>
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
	return write_some(buf.c_str(), buf.size(), error);
}

std::size_t tcp_socket::write_some(const void *buf, std::size_t size, asio::error_code &error)
{
	m_sock->non_blocking(false);
	const char *cbuf = static_cast<const char*>(buf);
	std::size_t counter = 0;

	while( size > 0 )
	{
		auto res = m_sock->write_some(asio::buffer(cbuf, size), error);
		if( error )
		{
			if( res > 0 )
				counter += res;
			break;
		}
		else
			counter += res;

		size -= res;
		cbuf += res;
	}
	return counter;
}

std::size_t tcp_socket::read_some(std::string &buf, asio::error_code &error)
{
	tcp::socket::receive_buffer_size option;
	get_option(option);

	m_sock->non_blocking(false);
	char *tmp = new char[option.value()] {0};
	auto res = m_sock->read_some(asio::buffer(tmp, option.value()), error);

	buf = std::string(tmp,res);
	delete[] tmp;
	return res;
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

void tcp_socket::async_read_some(std::string &buf, std::function<void(asio::error_code)> callback)
{
	tcp::socket::receive_buffer_size option;
	get_option(option);

	char *tmp = new char[option.value()] {0};
	m_sock->async_read_some(asio::buffer(tmp, option.value()), [&buf, callback, tmp](const asio::error_code &error, std::size_t size)
	{
		buf = std::string(tmp,size);
		delete[] tmp;
		callback(error);
	});
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
		this->error(error, "write_some(std::string)");
	return res;
}

std::size_t tcp_socket::write_some(const void *buf, std::size_t size)
{
	asio::error_code error;
	auto res = write_some(buf, size, error);
	if( error )
		this->error(error, "write_some(void*)");
	return res;
}

std::size_t tcp_socket::read_some(std::string &buf)
{
	asio::error_code error;
	auto res = read_some(buf, error);
	if( error )
		this->error(error, "read_some(std::string)");
	return res;
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size)
{
	asio::error_code error;
	auto res = read_some(buf, size, error);
	if( error )
		this->error(error, "read_some(void*)");
	return res;
}

void tcp_socket::async_write_some(const std::string &buf, std::function<void(std::size_t)> callback)
{
	async_write_some(buf, [callback](const asio::error_code &error, std::size_t size)
	{
		if( error )
			tcp_socket::error(error, "async_write_some(std::string)");
		callback(size);
	});
}

void tcp_socket::async_write_some(const void *buf, std::size_t size, std::function<void(std::size_t)> callback)
{
	async_write_some(buf, size, [callback](const asio::error_code &error, std::size_t size)
	{
		if( error )
			tcp_socket::error(error, "async_write_some(void*)");
		callback(size);
	});
}

void tcp_socket::async_read_some(std::string &buf, std::function<void()> callback)
{
	async_read_some(buf, [callback](const asio::error_code &error)
	{
		if( error )
			tcp_socket::error(error, "async_read_some(std::string)");
		callback();
	});
}

void tcp_socket::async_read_some(void *buf, std::size_t size, std::function<void(std::size_t)> callback)
{
	async_read_some(buf, size, [callback](const asio::error_code &error, std::size_t size)
	{
		if( error )
			tcp_socket::error(error, "async_read_some(void*)");
		callback(size);
	});
}

std::size_t tcp_socket::read_some(std::string &buf, const duration &timeout, asio::error_code &error)
{
	if( wait_readable(timeout, error) )
		return read_some(buf, error);
	return 0;
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size, const duration &timeout, asio::error_code &error)
{
	if( wait_readable(timeout, error) )
		return read_some(buf, size, error);
	return 0;
}

std::size_t tcp_socket::read_some(std::string &buf, const duration &timeout)
{
	asio::error_code error;
	if( wait_readable(timeout, error) )
		return read_some(buf);
	else if( error )
		this->error(error, "read_some: wait_readable");
	return 0;
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size, const duration &timeout)
{
	asio::error_code error;
	if( wait_readable(timeout, error) )
		return read_some(buf, size);
	else if( error )
		this->error(error, "read_some: wait_readable");
	return 0;
}

tcp::endpoint tcp_socket::remote_endpoint(asio::error_code &error)
{
	return m_sock->remote_endpoint(error);
}

tcp::endpoint tcp_socket::remote_endpoint()
{
	asio::error_code error;
	auto res = m_sock->remote_endpoint(error);
	if( error )
		this->error(error, "remote_endpoint");
	return res;
}

tcp::endpoint tcp_socket::local_endpoint(asio::error_code &error)
{
	return m_sock->local_endpoint(error);
}

tcp::endpoint tcp_socket::local_endpoint()
{
	asio::error_code error;
	auto res = m_sock->local_endpoint(error);
	if( error )
		this->error(error, "local_endpoint");
	return res;
}

void tcp_socket::shutdown(asio::error_code &error, tcp::socket::shutdown_type what)
{
	m_sock->shutdown(what, error);
}

void tcp_socket::shutdown(tcp::socket::shutdown_type what)
{
	asio::error_code error;
	m_sock->shutdown(what, error);
	if( error )
		this->error(error, "shutdown");
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
	asio::error_code error;
	if( _shutdown )
	{
		shutdown(error, tcp::socket::shutdown_both);
		if( error )
			this->error(error, "close: shutdown");
	}
	m_sock->close(error);
	if( error )
		this->error(error, "close");
}

void tcp_socket::non_blocking(bool mode, asio::error_code &error)
{
	m_sock->non_blocking(mode, error);
}

void tcp_socket::non_blocking(bool mode)
{
	asio::error_code error;
	m_sock->non_blocking(mode, error);
	if( error )
		this->error(error, "non_blocking");
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

void tcp_socket::error(const asio::error_code &error, const char *func)
{
	std::cerr << fmt::format("*** Error: tcp_socket::{}: {}", func, error) << std::endl;
}

} //namespace gts
