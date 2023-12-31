
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include "ssl_socket.h"
#ifdef GTS_ENABLE_SSL

#include "coro_for_asio.h"

GTS_NAMESPACE_BEGIN

ssl_socket::ssl_socket(ssl_stream *sock) :
	tcp_socket(&sock->next_layer()),
	m_ssl_sock(sock)
{

}

ssl_socket::~ssl_socket() noexcept
{
	m_sock = nullptr;
}

std::size_t ssl_socket::write_some(const void *buf, std::size_t size, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;

	const char *cbuf = static_cast<const char*>(buf);
	std::size_t counter = 0;

	while( size > 0 )
	{
		auto res = m_ssl_sock->write_some(asio::buffer(cbuf, size), error);
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

std::size_t ssl_socket::read_some(std::string &buf, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;

	tcp::socket::receive_buffer_size option; get_option(option);
	return m_ssl_sock->read_some(asio::buffer(buf, option.value()), error);
}

std::size_t ssl_socket::read_some(std::string &buf, std::size_t size, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;
	return m_ssl_sock->read_some(asio::buffer(buf, size), error);
}

std::size_t ssl_socket::read_some(void *buf, std::size_t size, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;
	return m_ssl_sock->read_some(asio::buffer(buf, size), error);
}

void ssl_socket::async_write_some(const std::string &buf, function_error_size callback) noexcept
{
	m_ssl_sock->async_write_some(asio::buffer(buf), std::move(callback));
}

void ssl_socket::async_write_some(const void *buf, std::size_t size, function_error_size callback) noexcept
{
	m_ssl_sock->async_write_some(asio::buffer(buf, size), std::move(callback));
}

void ssl_socket::async_read_some(std::string &buf, function_error callback) noexcept
{
	tcp::socket::receive_buffer_size option;
	get_option(option);

	char *tmp = new char[option.value()] {0};
	auto callback_mw = make_move_wrapper(std::move(callback));

	m_ssl_sock->async_read_some(asio::buffer(tmp, option.value()), [&buf, tmp, callback_mw](const asio::error_code &error, std::size_t size)
	{
		buf = std::string(tmp,size);
		delete[] tmp;
		(*callback_mw)(error);
	});
}

void ssl_socket::async_read_some(std::string &buf, std::size_t size, function_error callback) noexcept
{
	if( size == 0 )
		return callback(asio::error_code());

	char *tmp = new char[size] {0};
	auto callback_mw = make_move_wrapper(std::move(callback));

	m_ssl_sock->async_read_some(asio::buffer(tmp, size), [&buf, tmp, callback_mw](const asio::error_code &error, std::size_t size)
	{
		buf = std::string(tmp,size);
		delete[] tmp;
		(*callback_mw)(error);
	});
}

void ssl_socket::async_read_some(void *buf, std::size_t size, function_error_size callback) noexcept
{
	m_ssl_sock->async_read_some(asio::buffer(buf, size), std::move(callback));
}

std::size_t ssl_socket::coro_await_write_some(const std::string &buf, asio::error_code &error) noexcept
{
	return coro_await_write(*m_ssl_sock, buf, error);
}

std::size_t ssl_socket::coro_await_write_some(const void *buf, std::size_t size, asio::error_code &error) noexcept
{
	return coro_await_write(*m_ssl_sock, buf, size, error);
}

std::size_t ssl_socket::coro_await_read_some(std::string &buf, asio::error_code &error) noexcept
{
	return coro_await_read(*m_ssl_sock, buf, error);
}

std::size_t ssl_socket::coro_await_read_some(std::string &buf, std::size_t size, asio::error_code &error) noexcept
{
	return coro_await_read(*m_ssl_sock, buf, size, error);
}

std::size_t ssl_socket::coro_await_read_some(void *buf, std::size_t size, asio::error_code &error) noexcept
{
	return coro_await_read(*m_ssl_sock, buf, size, error);
}

ssl::context &ssl_socket::asio_ssl_context()
{
	static ssl::context ssl_context { ssl::context::sslv23_server };
	return ssl_context;
}

GTS_NAMESPACE_END

#endif //GTS_ENABLE_SSL
