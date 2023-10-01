
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

#ifndef GTS_SSL_SOCKET_H
#define GTS_SSL_SOCKET_H

#include <gts/tcp_socket.h>

#ifdef GTS_ENABLE_SSL
#include <asio/ssl.hpp>

namespace ssl = asio::ssl;

namespace gts
{

using ssl_stream = ssl::stream<tcp::socket>;

class GTSCORE_API ssl_socket : public tcp_socket
{
	GTS_DISABLE_COPY_MOVE(ssl_socket)

public:
	explicit ssl_socket(ssl_stream *sock);
	~ssl_socket() noexcept override;

public:
	std::size_t write_some(const void *buf, std::size_t size, asio::error_code &error) noexcept override;
	std::size_t read_some(std::string &buf, asio::error_code &error) noexcept override;
	std::size_t read_some(void *buf, std::size_t size, asio::error_code &error) noexcept override;

public:
	void async_write_some(const std::string &buf, std::function<void(asio::error_code, std::size_t)>) noexcept override;
	void async_write_some(const void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>) noexcept override;
	void async_read_some(std::string &buf, std::function<void(asio::error_code)>) noexcept override;
	void async_read_some(std::string &buf, std::size_t size, std::function<void(asio::error_code)>) noexcept override;
	void async_read_some(void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>) noexcept override;

public:
	static ssl::context &asio_ssl_context();

protected:
	ssl_stream *m_ssl_sock;
};

} //namespace gts

#endif //ssl

#endif //GTS_SSL_SOCKET_H
