
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

#ifndef GTS_WEB_SOCKET_FRAME_PARSER_H
#define GTS_WEB_SOCKET_FRAME_PARSER_H

#include <gts/web/socket_frame.h>

GTS_WEB_NAMESPACE_BEGIN

class socket_frame_parser_impl;

class GTS_WEB_API socket_frame_parser
{
	GTS_DISABLE_COPY_MOVE(socket_frame_parser)

public:
	socket_frame_parser() = default;
	bool append(const std::string &data);

public:
	GTS_CXX_NODISCARD("") bool is_done() const;
	GTS_CXX_NODISCARD("") socket_frame next_frame() const;

public:
	void clear();

public:
	void set_max_allowed_frame_size(std::size_t size);
	GTS_CXX_NODISCARD("") std::size_t max_allowed_frame_size() const;
	GTS_CXX_NODISCARD("") static std::size_t max_frame_size();

private:
	socket_frame_parser_impl *m_impl;
};

GTS_WEB_NAMESPACE_END

GTS_NAMESPACE_BEGIN
using websocket_frame_parser = web::socket_frame_parser;
GTS_NAMESPACE_END


#endif //GTS_WEB_SOCKET_FRAME_PARSER_H
