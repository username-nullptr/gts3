
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

#ifndef GTS_TCP_SOCKET_H
#define GTS_TCP_SOCKET_H

#include <gts/global.h>
#include <functional>

using namespace asio::ip;

GTS_NAMESPACE_BEGIN

class GTSCORE_API tcp_socket
{
	GTS_DISABLE_COPY_MOVE(tcp_socket)
	using duration = std::chrono::milliseconds;

protected:
	using function_void = std::function<void()>;
	using function_size = std::function<void(std::size_t)>;
	using function_error = std::function<void(asio::error_code)>;
	using function_error_size = std::function<void(asio::error_code,std::size_t)>;

public:
	explicit tcp_socket(tcp::socket *sock);
	virtual ~tcp_socket() noexcept;

public:
	virtual std::size_t write_some(const std::string &buf, asio::error_code &error) noexcept;
	virtual std::size_t write_some(const void *buf, std::size_t size, asio::error_code &error) noexcept;

	virtual std::size_t read_some(std::string &buf, asio::error_code &error) noexcept;
	virtual std::size_t read_some(std::string &buf, std::size_t size, asio::error_code &error) noexcept;
	virtual std::size_t read_some(void *buf, std::size_t size, asio::error_code &error) noexcept;

public:
	virtual void async_write_some(const std::string &buf, function_error_size callback) noexcept;
	virtual void async_write_some(const void *buf, std::size_t size, function_error_size callback) noexcept;

	virtual void async_read_some(std::string &buf, function_error callback) noexcept;
	virtual void async_read_some(std::string &buf, std::size_t, function_error callback) noexcept;
	virtual void async_read_some(void *buf, std::size_t size, function_error_size callback) noexcept;

public:
	std::size_t write_some(const std::string &buf) noexcept;
	std::size_t write_some(const void *buf, std::size_t size) noexcept;

	std::size_t read_some(std::string &buf) noexcept;
	std::size_t read_some(std::string &buf, std::size_t size) noexcept;
	std::size_t read_some(void *buf, std::size_t size) noexcept;

public:
	void async_write_some(const std::string &buf, function_size callback) noexcept;
	void async_write_some(const void *buf, std::size_t size, function_size callback) noexcept;

	void async_read_some(std::string &buf, function_void callback) noexcept;
	void async_read_some(std::string &buf, std::size_t, function_void callback) noexcept;
	void async_read_some(void *buf, std::size_t size, function_size callback) noexcept;

public:
	void write_some_nonblock(std::string buf) noexcept;
	void write_some_nonblock(const void *buf, std::size_t size) noexcept;

public:
	std::size_t read_some(std::string &buf, const duration &timeout, asio::error_code &error) noexcept;
	std::size_t read_some(std::string &buf, std::size_t size, const duration &timeout, asio::error_code &error) noexcept;
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout, asio::error_code &error) noexcept;

	std::size_t read_some(std::string &buf, const duration &timeout) noexcept;
	std::size_t read_some(std::string &buf, std::size_t size, const duration &timeout) noexcept;
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout) noexcept;

public:
	bool wait_writeable(const duration &ms, asio::error_code &error) noexcept;
	bool wait_readable(const duration &ms, asio::error_code &error) noexcept;
	bool wait_writeable(const duration &ms) noexcept;
	bool wait_readable(const duration &ms) noexcept;

public:
	GTS_CXX_NODISCARD("") tcp::endpoint remote_endpoint(asio::error_code &error) noexcept;
	GTS_CXX_NODISCARD("") tcp::endpoint remote_endpoint() noexcept;

public:
	GTS_CXX_NODISCARD("") tcp::endpoint local_endpoint(asio::error_code &error) noexcept;
	GTS_CXX_NODISCARD("") tcp::endpoint local_endpoint() noexcept;

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
	GTS_CXX_NODISCARD("") bool is_open() const;
	void close(asio::error_code &error, bool shutdown = false) noexcept;
	void close(bool shutdown = false) noexcept;

public:
	void non_blocking(bool mode, asio::error_code& error) noexcept;
	void non_blocking(bool mode) noexcept;

	GTS_CXX_NODISCARD("")
	bool non_blocking() const noexcept;

public:
	GTS_CXX_NODISCARD("Get const asio::tcp::socket")
	const tcp::socket &native() const noexcept;

	GTS_CXX_NODISCARD("Get asio::tcp::socket")
	tcp::socket &native() noexcept;

protected:
	static void error(const asio::error_code &error, const char *func) noexcept;
	tcp::socket *m_sock;
};

using tcp_socket_ptr = std::shared_ptr<tcp_socket>;

GTS_NAMESPACE_END
#include <gts/detail/tcp_socket.h>

#endif //GTS_TCP_SOCKET_H
