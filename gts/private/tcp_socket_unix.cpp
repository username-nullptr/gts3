
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

#ifdef __unix__

#include "gts/tcp_socket.h"
#include <poll.h>

GTS_NAMESPACE_BEGIN

static bool wait(int fd, int event, const std::chrono::milliseconds &ms, asio::error_code &error)
{
	struct pollfd fds;
	fds.fd = fd;
	fds.events = event;

	int tt = ms.count() > 0? ms.count() : -1;
	error = std::make_error_code(static_cast<std::errc>(0));

	for(;;)
	{
		int res = poll(&fds, 1, tt);
		if( res < 0 )
		{
			error = std::make_error_code(static_cast<std::errc>(errno));
			return false;
		}
		else if( res == 0 )
			return false;
		else if( res == 1 and fds.revents == event )
			break;
	}
	return true;
}

bool tcp_socket::wait_writeable(const duration &ms, asio::error_code &error) noexcept
{
	return wait(m_sock->native_handle(), POLLOUT, ms, error);
}

bool tcp_socket::wait_readable(const duration &ms, asio::error_code &error) noexcept
{
	return wait(m_sock->native_handle(), POLLIN, ms, error);
}

bool tcp_socket::wait_writeable(const duration &ms) noexcept
{
	asio::error_code error ;
	bool res = wait(m_sock->native_handle(), POLLOUT, ms, error);
	if( error )
		this->error(error, "wait_writeable");
	return res;
}

bool tcp_socket::wait_readable(const duration &ms) noexcept
{
	asio::error_code error ;
	bool res = wait(m_sock->native_handle(), POLLIN, ms, error);
	if( error )
		this->error(error, "wait_readable");
	return res;
}

GTS_NAMESPACE_END

#endif //__unix__
