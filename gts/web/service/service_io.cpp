
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

#include "service_io.h"
#include "gts/http/detail/request_impl.h"

GTS_WEB_NAMESPACE_BEGIN

service_io::service_io(http::service_context &context) :
	context(context), socket(context.socket())
{
	auto &request = this->request();
	auto &response = this->response();

	response.set_header("server", "gts3");
	auto it = request.headers().find("origin");

	if( it != request.headers().end() )
		response.set_header("access-control-allow-origin", it->second);

	if( request.version() == "1.1" )
	{
		if( not request.keep_alive() )
			response.set_header("connection", "close");
	}
}

void service_io::return_to_null(http::status status)
{
	auto body = fmt::format("{} ({})", http::status_description(status), status);

	response().set_header("content-length", body.size())
			.set_status(status)
			.write(body);

	if( not request().keep_alive() )
		response().close(true);
}

http::request &service_io::request()
{
	return context.request();
}

http::response &service_io::response()
{
	return context.response();
}

GTS_WEB_NAMESPACE_END
