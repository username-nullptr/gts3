
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

#ifndef GTS_HTTP_REQUEST_IMPL_H
#define GTS_HTTP_REQUEST_IMPL_H

#include "gts/http/request.h"
#include "gts/http/response.h"
#include "gts/http/session.h"
#include "gts/tcp_socket.h"

GTS_HTTP_NAMESPACE_BEGIN

class GTS_DECL_HIDDEN request_impl
{
	GTS_DISABLE_COPY_MOVE(request_impl)

public:
	request_impl() = default;

public:
	tcp_socket &socket() const;
	std::size_t tcp_ip_buffer_size() const;
	void finish(std::string &body);

public:
	std::size_t read_body_length_mode(std::error_code &error, void *buf, std::size_t size);
	std::size_t read_body_chunked_mode(std::error_code &error, void *buf, std::size_t size);

public:
	bool m_keep_alive = true;
	bool m_support_gzip = false;
	bool m_websocket = false;

	enum class rb_status
	{
		length,
		wait_size,
		wait_content,
		wait_headers,
		chunked_end,
		finished
	}
	m_rb_status = rb_status::length;
	rttr::variant m_rb_context;

public:
	response *m_response = nullptr;
	tcp_socket_ptr m_socket;

	http::method m_method;
	std::string m_version;
	std::string m_path;

	http::parameters m_parameters;
	http::headers m_headers;
	http::basic_cookies m_cookies;

	std::string m_parameters_string;
	std::string m_body;
};

GTS_HTTP_NAMESPACE_END


#endif //GTS_HTTP_REQUEST_IMPL_H
