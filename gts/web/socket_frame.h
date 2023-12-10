
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

#ifndef GTS_WEB_SOCKET_FRAME_H
#define GTS_WEB_SOCKET_FRAME_H

#include <gts/web/types.h>

GTS_WEB_NAMESPACE_BEGIN

class socket_frame_parser_impl;
class socket_frame_parser;
class socket_frame_impl;

class GTS_WEB_API socket_frame
{
	GTS_DISABLE_COPY(socket_frame)
	using sp_close_code = socket_protocol::close_code;
	using sp_op_code = socket_protocol::op_code;

public:
	socket_frame();
	~socket_frame();

public:
	socket_frame(socket_frame &&other) noexcept;
	socket_frame &operator=(socket_frame &&other) noexcept;
	socket_frame &swap(socket_frame &other);

public:
	GTS_CXX_NODISCARD("") bool is_final_frame() const;
	GTS_CXX_NODISCARD("") bool is_control_frame() const;
	GTS_CXX_NODISCARD("") bool is_data_frame() const;
	GTS_CXX_NODISCARD("") bool is_continuation_frame() const;

public:
	GTS_CXX_NODISCARD("") bool rsv1() const;
	GTS_CXX_NODISCARD("") bool rsv2() const;
	GTS_CXX_NODISCARD("") bool rsv3() const;

public:
	GTS_CXX_NODISCARD("") bool has_mask() const;
	GTS_CXX_NODISCARD("") uint32_t mask() const;

public:
	GTS_CXX_NODISCARD("") sp_close_code close_code() const;
	GTS_CXX_NODISCARD("") sp_op_code op_code() const;
	GTS_CXX_NODISCARD("") std::string payload() const;

public:
	GTS_CXX_NODISCARD("") uint64_t length() const;
	GTS_CXX_NODISCARD("") bool is_valid() const;

private:
	friend class socket_frame_parser_impl;
	friend class socket_frame_parser;
	socket_frame_impl *m_impl;
};

GTS_WEB_NAMESPACE_END

GTS_NAMESPACE_BEGIN
using websocket_frame = web::socket_frame;
GTS_NAMESPACE_END


#endif //GTS_WEB_SOCKET_FRAME_H
