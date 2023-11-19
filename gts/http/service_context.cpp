
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

#include "service_context.h"
#include "detail/request_impl.h"

GTS_HTTP_NAMESPACE_BEGIN

class GTS_DECL_HIDDEN service_context_impl
{
	GTS_DISABLE_COPY_MOVE(service_context_impl)

public:
	service_context_impl(http::request *request) :
		m_requst(request), m_response(*request)
	{
		assert(request);
		m_requst->m_impl->m_response = &m_response;
	}

	~service_context_impl() {
		delete_later(m_requst);
	}

public:
	request *m_requst;
	response m_response;
};

/*-------------------------------------------------------------------------------------------------------------------------------------*/

service_context::service_context(http::request *request) :
	m_impl(new service_context_impl(request))
{

}

service_context::~service_context()
{
	delete m_impl;
}

request &service_context::request()
{
	return *m_impl->m_requst;
}

response &service_context::response()
{
	return m_impl->m_response;
}

tcp_socket &service_context::socket()
{
	return m_impl->m_requst->m_impl->socket();
}

bool service_context::is_valid() const
{
	return m_impl->m_requst->is_valid();
}

GTS_HTTP_NAMESPACE_END
