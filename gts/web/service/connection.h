
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

#ifndef GTS_WEB_CONNECTION_H
#define GTS_WEB_CONNECTION_H

#include <gts/web/service/task.h>
#include <gts/http/parser.h>

GTS_WEB_NAMESPACE_BEGIN

class GTS_DECL_HIDDEN connection
{
	GTS_DISABLE_COPY_MOVE(connection)
	connection(tcp_socket_ptr socket);

public:
	~connection();
	void cancel();

public:
	static void init();
	static void new_connection(tcp_socket_ptr socket);
	static std::string view_status();

private:
	void do_recv();
	void time_out_allow_preemptionx(const asio::error_code &was_cancel);
	void time_out_destory(const asio::error_code &was_cancel);

private:
	tcp_socket_ptr m_socket;
	asio::steady_timer m_timer;

private:
	char *m_asio_buffer = nullptr;
	std::size_t m_ab_size = 65536;

private:
	http::parser *m_parser = nullptr;
	task m_task;
};

GTS_WEB_NAMESPACE_END


#endif //GTS_WEB_CONNECTION_H

