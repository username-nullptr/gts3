
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

#ifndef GTS_HTTP_SERVICE_CONTEXT_H
#define GTS_HTTP_SERVICE_CONTEXT_H

#include <gts/http/request.h>
#include <gts/http/response.h>

GTS_HTTP_NAMESPACE_BEGIN

class service_context_impl;

class GTS_HTTP_API service_context
{
	GTS_DISABLE_COPY_MOVE(service_context)

public:
	explicit service_context(http::request *request);
	~service_context();

public:
	GTS_CXX_NODISCARD("") http::request &request();
	GTS_CXX_NODISCARD("") http::response &response();

public:
	GTS_CXX_NODISCARD("") tcp_socket &socket();
	GTS_CXX_NODISCARD("") bool is_valid() const;

public:
	GTS_CXX_NODISCARD("") std::string &body_cache();

private:
	service_context_impl *m_impl;
};
using service_context_ptr = std::shared_ptr<service_context>;

GTS_HTTP_NAMESPACE_END


#endif //GTS_HTTP_SERVICE_CONTEXT_H
