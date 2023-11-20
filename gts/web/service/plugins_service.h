
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

#ifndef GTS_WEB_PLUGIN_SERVICE_H
#define GTS_WEB_PLUGIN_SERVICE_H

#include <gts/web/registration.h>

GTS_WEB_NAMESPACE_BEGIN

class service_io;

class GTS_DECL_HIDDEN plugins_service
{
	GTS_DISABLE_COPY_MOVE(plugins_service)

public:
	explicit plugins_service(service_io &sio);
	GTS_CXX_NODISCARD("") bool exists();

public:
	GTS_CXX_NODISCARD("") bool call();
	GTS_CXX_NODISCARD("") bool call_filter();

private:
	registration::service_array *check();
	rttr::variant method_call(rttr::method &method, const rttr::instance &obj, const rttr::type &p1_type);

private:
	service_io &m_sio;
};

GTS_WEB_NAMESPACE_END


#endif //GTS_WEB_PLUGIN_SERVICE_H
