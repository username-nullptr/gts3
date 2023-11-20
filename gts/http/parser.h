
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

#ifndef GTS_HTTP_PARSER_H
#define GTS_HTTP_PARSER_H

#include <gts/http/service_context.h>
#include <gts/tcp_socket.h>

GTS_HTTP_NAMESPACE_BEGIN

class parser_impl;

class GTS_HTTP_API parser
{
	GTS_DISABLE_COPY_MOVE(parser)

public:
	explicit parser(std::size_t buf_size = 65536);
	~parser();

public:
	GTS_CXX_NODISCARD("Returns true if the parsing is complete")
	bool write(const std::string &data);

	GTS_CXX_NODISCARD("Get the context of this service")
	service_context_ptr get_request(tcp_socket_ptr socket);

private:
	parser_impl *m_impl;
};

GTS_HTTP_NAMESPACE_END


#endif //GTS_HTTP_PARSER_H
