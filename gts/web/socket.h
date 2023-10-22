
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

#ifndef GTS_WEB_SOCKET_H
#define GTS_WEB_SOCKET_H

#include <gts/web/types.h>
#include <gts/http/request.h>
#include <gts/http/response.h>

// Not Implemented
namespace gts { namespace web
{

class socket_impl;

class GTS_WEB_API socket
{
	GTS_DISABLE_COPY_MOVE(socket)
	using buffer = socket_protocol::buffer;
	using duration = std::chrono::milliseconds;

public:
	socket(const http::request &req, http::response &resp, const std::string &sec_websocket_protocol = "") noexcept(false);
	virtual ~socket() noexcept;

public:
	socket_protocol::version version() const;

public:
	std::size_t write_some(const buffer &buf, asio::error_code &error) noexcept;
	std::size_t write_some(const buffer &buf) noexcept;

	std::size_t read_some(buffer &buf, asio::error_code &error) noexcept;
	std::size_t read_some(buffer &buf) noexcept;

	std::size_t read_some(buffer &buf, const duration &timeout, asio::error_code &error) noexcept;
	std::size_t read_some(buffer &buf, const duration &timeout) noexcept;

public:
	void async_write_some(const buffer &buf, std::function<void(asio::error_code, std::size_t)>) noexcept;
	void async_write_some(const buffer &buf, std::function<void(std::size_t)>) noexcept;

	void async_read_some(buffer &buf, std::function<void(asio::error_code)>) noexcept;
	void async_read_some(buffer &buf, std::function<void()>) noexcept;

public:
	void write_some_nonblock(buffer buf) noexcept;

public:
	bool wait_writeable(const duration &ms, asio::error_code &error) noexcept;
	bool wait_writeable(const duration &ms) noexcept;

	bool wait_readable(const duration &ms, asio::error_code &error) noexcept;
	bool wait_readable(const duration &ms) noexcept;

public:
	tcp::endpoint remote_endpoint(asio::error_code &error) noexcept;
	tcp::endpoint remote_endpoint() noexcept;

public:
	tcp::endpoint local_endpoint(asio::error_code &error) noexcept;
	tcp::endpoint local_endpoint() noexcept;

public:
	bool is_open() const;
	void close(asio::error_code &error, bool shutdown = false) noexcept;
	void close(bool shutdown = false) noexcept;

public:
	const tcp_socket_ptr native() const noexcept;
	tcp_socket_ptr native() noexcept;

private:
	socket_impl *m_impl;
};

using socket_ptr = std::shared_ptr<socket>;

template <typename...Args>
socket_ptr make_socket_ptr(Args&&...args);

} //namespace web

using websocket = web::socket;

using websocket_ptr = web::socket_ptr;

template <typename...Args>
websocket_ptr make_websocket_ptr(Args&&...args);

} //namespace gts

#include <gts/web/detail/socket.h>

#endif //GTS_WEB_SOCKET_H
