
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

#ifndef GTS_DETAIL_TCP_SOCKET_H
#define GTS_DETAIL_TCP_SOCKET_H

namespace gts
{

template <typename SettableSocketOption>
void tcp_socket::set_option(const SettableSocketOption &option, asio::error_code &error) noexcept
{
	m_sock->set_option(option, error);
}

template <typename SettableSocketOption>
void tcp_socket::set_option(const SettableSocketOption &option) noexcept
{
	asio::error_code error;
	m_sock->set_option(option, error);
	if( error )
		this->error(error, "set_option");
}

template <typename GettableSocketOption>
void tcp_socket::get_option(GettableSocketOption &option, asio::error_code &error) const  noexcept
{
	m_sock->get_option(option, error);
}

template <typename GettableSocketOption>
void tcp_socket::get_option(GettableSocketOption &option) const noexcept
{
	asio::error_code error;
	m_sock->get_option(option, error);
	if( error )
		this->error(error, "get_option");
}

} //namespace gts


#endif //GTS_DETAIL_TCP_SOCKET_H
