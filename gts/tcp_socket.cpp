
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

#include "tcp_socket.h"
#include "gts/log.h"
#include <cppformat>

GTS_NAMESPACE_BEGIN

tcp_socket::tcp_socket(tcp::socket *sock) :
	m_sock(sock)
{
	assert(sock);
}

tcp_socket::~tcp_socket() noexcept
{
	if( m_sock )
		delete m_sock;
}

std::size_t tcp_socket::write_some(const std::string &buf, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;
	return write_some(buf.c_str(), buf.size(), error);
}

std::size_t tcp_socket::write_some(const void *buf, std::size_t size, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;

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

std::size_t tcp_socket::read_some(std::string &buf, asio::error_code &error) noexcept
{
	tcp::socket::receive_buffer_size option;
	get_option(option);

	non_blocking(false, error);
	if( error )
		return 0;

	char *tmp = new char[option.value()] {0};
	auto res = m_sock->read_some(asio::buffer(tmp, option.value()), error);

	buf = std::string(tmp,res);
	delete[] tmp;
	return res;
}

std::size_t tcp_socket::read_some(std::string &buf, std::size_t size, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;
	return m_sock->read_some(asio::buffer(buf, size), error);
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size, asio::error_code &error) noexcept
{
	non_blocking(false, error);
	if( error )
		return 0;
	return m_sock->read_some(asio::buffer(buf, size), error);
}

void tcp_socket::async_write_some(const std::string &buf, function_error_size callback) noexcept
{
	auto callback_mw = make_move_wrapper(std::move(callback));
	m_sock->async_write_some(asio::buffer(buf), [this, &buf, callback_mw](const asio::error_code &error, std::size_t size)
	{
		auto callback = callback_mw.take();
		if( error or size >= buf.size() )
			callback(error, size);
		else
			async_write_some(buf.c_str() + size, buf.size() - size, std::move(callback));
	});
}

void tcp_socket::async_write_some(const void *buf, std::size_t size, function_error_size callback) noexcept
{
	auto callback_mw = make_move_wrapper(std::move(callback));
	m_sock->async_write_some(asio::buffer(buf, size), [this, buf, size, callback_mw](const asio::error_code &error, std::size_t rsize)
	{
		auto callback = callback_mw.take();
		if( error or rsize >= size )
			callback(error, rsize);
		else
			async_write_some(reinterpret_cast<const char*>(buf) + rsize, size - rsize, std::move(callback));
	});
}

void tcp_socket::async_read_some(std::string &buf, function_error callback) noexcept
{
	tcp::socket::receive_buffer_size option;
	get_option(option);

	char *tmp = new char[option.value()] {0};
	auto callback_mw = make_move_wrapper(std::move(callback));

	m_sock->async_read_some(asio::buffer(tmp, option.value()), [&buf, tmp, callback_mw](const asio::error_code &error, std::size_t size)
	{
		buf = std::string(tmp,size);
		delete[] tmp;
		(*callback_mw)(error);
	});
}

void tcp_socket::async_read_some(std::string &buf, std::size_t size, function_error callback) noexcept
{
	if( size == 0 )
		return callback(asio::error_code());

	char *tmp = new char[size] {0};
	auto callback_mw = make_move_wrapper(std::move(callback));

	m_sock->async_read_some(asio::buffer(tmp, size), [&buf, tmp, callback_mw](const asio::error_code &error, std::size_t size)
	{
		buf = std::string(tmp,size);
		delete[] tmp;
		(*callback_mw)(error);
	});
}

void tcp_socket::async_read_some(void *buf, std::size_t size, function_error_size callback) noexcept
{
	m_sock->async_read_some(asio::buffer(buf, size), std::move(callback));
}

std::size_t tcp_socket::write_some(const std::string &buf) noexcept
{
	asio::error_code _error;
	auto res = write_some(buf, _error);
	if( _error )
		error(_error, "write_some(std::string)");
	return res;
}

std::size_t tcp_socket::write_some(const void *buf, std::size_t size) noexcept
{
	asio::error_code _error;
	auto res = write_some(buf, size, _error);
	if( _error )
		error(_error, "write_some(void*)");
	return res;
}

std::size_t tcp_socket::read_some(std::string &buf) noexcept
{
	asio::error_code _error;
	auto res = read_some(buf, _error);
	if( _error )
		error(_error, "read_some(std::string)");
	return res;
}

std::size_t tcp_socket::read_some(std::string &buf, std::size_t size) noexcept
{
	asio::error_code _error;
	auto res = read_some(buf, size, _error);
	if( _error )
		error(_error, "read_some(void*)");
	return res;
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size) noexcept
{
	asio::error_code _error;
	auto res = read_some(buf, size, _error);
	if( _error )
		error(_error, "read_some(void*)");
	return res;
}

void tcp_socket::async_write_some(const std::string &buf, function_size callback) noexcept
{
	auto callback_mw = make_move_wrapper(std::move(callback));
	async_write_some(buf, [callback_mw](const asio::error_code &error, std::size_t size)
	{
		if( error )
			tcp_socket::error(error, "async_write_some(std::string)");
		(*callback_mw)(size);
	});
}

void tcp_socket::async_write_some(const void *buf, std::size_t size, function_size callback) noexcept
{
	auto callback_mw = make_move_wrapper(std::move(callback));
	async_write_some(buf, size, [callback_mw](const asio::error_code &error, std::size_t size)
	{
		if( error )
			tcp_socket::error(error, "async_write_some(void*)");
		(*callback_mw)(size);
	});
}

void tcp_socket::async_read_some(std::string &buf, function_void callback) noexcept
{
	auto callback_mw = make_move_wrapper(std::move(callback));
	async_read_some(buf, [callback_mw](const asio::error_code &error)
	{
		if( error )
			tcp_socket::error(error, "async_read_some(std::string)");
		(*callback_mw)();
	});
}

void tcp_socket::async_read_some(std::string &buf, std::size_t size, function_void callback) noexcept
{
	auto callback_mw = make_move_wrapper(std::move(callback));
	async_read_some(buf, size, [callback_mw](const asio::error_code &error)
	{
		if( error )
			tcp_socket::error(error, "async_read_some(std::string,std::size_t)");
		(*callback_mw)();
	});
}

void tcp_socket::async_read_some(void *buf, std::size_t size, function_size callback) noexcept
{
	auto callback_mw = make_move_wrapper(std::move(callback));
	async_read_some(buf, size, [callback_mw](const asio::error_code &error, std::size_t size)
	{
		if( error )
			tcp_socket::error(error, "async_read_some(void*)");
		(*callback_mw)(size);
	});
}

void tcp_socket::write_some_nonblock(std::string buf) noexcept
{
	auto buf_ptr = std::make_shared<std::string>(std::move(buf));
	async_write_some(*buf_ptr, [buf_ptr](std::size_t){});
}

void tcp_socket::write_some_nonblock(const void *buf, std::size_t size) noexcept
{
	auto buf_ptr = std::make_shared<std::string>(reinterpret_cast<const char*>(buf), size);
	async_write_some(*buf_ptr, [buf_ptr](std::size_t){});
}

std::size_t tcp_socket::read_some(std::string &buf, const duration &timeout, asio::error_code &error) noexcept
{
	if( wait_readable(timeout, error) )
		return read_some(buf, error);
	return 0;
}

std::size_t tcp_socket::read_some(std::string &buf, std::size_t size, const duration &timeout, asio::error_code &error) noexcept
{
	if( wait_readable(timeout, error) )
		return read_some(buf, size, error);
	return 0;
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size, const duration &timeout, asio::error_code &error) noexcept
{
	if( wait_readable(timeout, error) )
		return read_some(buf, size, error);
	return 0;
}

std::size_t tcp_socket::read_some(std::string &buf, const duration &timeout) noexcept
{
	asio::error_code _error;
	if( wait_readable(timeout, _error) )
		return read_some(buf);
	else if( _error )
		error(_error, "read_some: wait_readable");
	return 0;
}

std::size_t tcp_socket::read_some(std::string &buf, std::size_t size, const duration &timeout) noexcept
{
	asio::error_code _error;
	if( wait_readable(timeout, _error) )
		return read_some(buf, size);
	else if( _error )
		error(_error, "read_some: wait_readable");
	return 0;
}

std::size_t tcp_socket::read_some(void *buf, std::size_t size, const duration &timeout) noexcept
{
	asio::error_code _error;
	if( wait_readable(timeout, _error) )
		return read_some(buf, size);
	else if( _error )
		error(_error, "read_some: wait_readable");
	return 0;
}

tcp::endpoint tcp_socket::remote_endpoint(asio::error_code &error) noexcept
{
	return m_sock->remote_endpoint(error);
}

tcp::endpoint tcp_socket::remote_endpoint() noexcept
{
	asio::error_code _error;
	auto res = m_sock->remote_endpoint(_error);
	if( _error )
		error(_error, "remote_endpoint");
	return res;
}

tcp::endpoint tcp_socket::local_endpoint(asio::error_code &error) noexcept
{
	return m_sock->local_endpoint(error);
}

tcp::endpoint tcp_socket::local_endpoint() noexcept
{
	asio::error_code _error;
	auto res = m_sock->local_endpoint(_error);
	if( _error )
		error(_error, "local_endpoint");
	return res;
}

void tcp_socket::shutdown(asio::error_code &error, tcp::socket::shutdown_type what) noexcept
{
	m_sock->shutdown(what, error);
}

void tcp_socket::shutdown(tcp::socket::shutdown_type what) noexcept
{
	asio::error_code _error;
	m_sock->shutdown(what, _error);
	if( _error )
		error(_error, "shutdown");
}

bool tcp_socket::is_open() const
{
	return m_sock->is_open();
}

void tcp_socket::close(asio::error_code &error, bool _shutdown) noexcept
{
	if( _shutdown )
		shutdown(error, tcp::socket::shutdown_both);
	m_sock->close(error);
}

void tcp_socket::close(bool _shutdown) noexcept
{
	asio::error_code _error;
	if( _shutdown )
	{
		shutdown(_error, tcp::socket::shutdown_both);
		if( _error )
			error(_error, "close: shutdown");
	}
	m_sock->close(_error);
	if( _error )
		error(_error, "close");
}

void tcp_socket::non_blocking(bool mode, asio::error_code &error) noexcept
{
	m_sock->non_blocking(mode, error);
}

void tcp_socket::non_blocking(bool mode) noexcept
{
	asio::error_code _error;
	m_sock->non_blocking(mode, _error);
	if( _error )
		error(_error, "non_blocking");
}

bool tcp_socket::non_blocking() const noexcept
{
	return m_sock->non_blocking();
}

const tcp::socket &tcp_socket::native() const noexcept
{
	return *m_sock;
}

tcp::socket &tcp_socket::native() noexcept
{
	return *m_sock;
}

void tcp_socket::error(const asio::error_code &error, const char *func) noexcept
{
	gts_log_error("gts::tcp_socket::{}: {}", func, error);
}

GTS_NAMESPACE_END
