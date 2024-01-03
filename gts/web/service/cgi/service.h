
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

#ifndef CGI_SERVICE_H
#define CGI_SERVICE_H

#include "process.h"
#include <gts/web/global.h>
#include <condition_variable>
#include <atomic>
#include <mutex>

GTS_WEB_NAMESPACE_BEGIN

#define BUF_SIZE  65536

class service_io;

class GTS_DECL_HIDDEN cgi_service
{
	GTS_DISABLE_COPY_MOVE(cgi_service)

public:
	explicit cgi_service(service_io &sio);
	~cgi_service();

public:
	static void init();
	std::string exists() const;
	bool call(std::string file_name = "");

//private:
//	void async_write_socket(const char *buf, std::size_t buf_size);
//	void async_read_socket();
//
//private:
//	void async_write_cgi(const char *buf, std::size_t buf_size);
//	void async_read_cgi();

private:
	void await_io_socket_to_cgi();
	void await_io_cgi_to_socket();

private:
	std::string replace_http_to_env(const std::string &str);

private:
	service_io &m_sio;
	process m_cgi;

private:
	char m_cgi_read_buf[BUF_SIZE];
//	std::size_t m_content_length = 0;
	std::size_t m_tcp_buf_size = BUF_SIZE;
	char *m_sock_read_buf = nullptr;

//private:
//	std::atomic_int m_counter {0};
//	std::condition_variable m_condition;
//	std::mutex m_mutex;
};

GTS_WEB_NAMESPACE_END


#endif //CGI_SERVICE_H
