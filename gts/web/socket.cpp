
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

#include "socket.h"
#include <gts/sha1.h>

namespace gts { namespace web
{

namespace sp = socket_protocol;

class GTS_DECL_HIDDEN socket_impl
{
	GTS_DISABLE_COPY_MOVE(socket_impl)

public:
	socket_impl() = default;
	tcp_socket_ptr m_socket;
	sp::version m_version = sp::version_unknown;
};

socket::socket(const http::request &req, http::response &resp, const std::string &sec_websocket_protocol) noexcept(false) :
	m_impl(new socket_impl())
{
	auto &headers = req.headers();
	auto it = headers.find(http::header::upgrade);

	if( it == headers.end() or str_to_lower(it->second) != "websocket" )
		throw exception("gts::websocket: Protocol upgrade is not supported.");

	it = headers.find("Sec-WebSocket-Key");
	if( it == headers.end() )
	{
		resp.set_status(http::hs_forbidden).write("'Sec-WebSocket-Key' not found.");
		throw exception("gts::websocket: http header 'Sec-WebSocket-Key' not found.");
	}
	auto &sw_key = it->second;
	auto swp_list = str_split(req.header_or("Sec-WebSocket-Protocol"), ",");

	for(auto &sswp : swp_list)
		sswp = str_trimmed(sswp);

	if( sec_websocket_protocol.empty() )
		resp.set_header("Sec-WebSocket-Protocol", swp_list.empty() ? "" : swp_list.front());
	else
	{
		auto it = std::find(swp_list.begin(), swp_list.end(), sec_websocket_protocol);
		if( it == swp_list.end() )
		{
			resp.set_status(http::hs_forbidden).write("'Sec-WebSocket-Protocol' not supported.");
			throw exception("gts::websocket: 'Sec-WebSocket-Protocol' not supported.");
		}
	}
	resp.set_header("Sec-WebSocket-Accept", sha1(sw_key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11").finalize().base64())
		.set_header(http::header::connection, "upgrade")
		.set_header(http::header::upgrade, "websocket")
		.set_status(http::hs_switching_protocols)
		.write();
	m_impl->m_version = req.header_or("Sec-WebSocket-Version", sp::version_unknown);
	m_impl->m_socket = resp.take();
}

socket::~socket() noexcept
{
	delete m_impl;
}

sp::version socket::version() const
{
	return m_impl->m_version;
}

bool socket::is_open() const
{
	return m_impl->m_socket != nullptr;
}

const tcp_socket_ptr socket::native() const noexcept
{
	return m_impl->m_socket;
}

tcp_socket_ptr socket::native() noexcept
{
	return m_impl->m_socket;
}

}} //namespace gts::web
