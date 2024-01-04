
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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "gts/gts_enable_ssl.h"

#ifdef GTS_ENABLE_SSL
# include "gts/ssl_socket.h"
#else
# include "gts/tcp_socket.h"
#endif

GTS_NAMESPACE_BEGIN

class tcp_server
{
	GTS_DISABLE_COPY_MOVE(tcp_server)

public:
	tcp_server();
	~tcp_server();

public:
	void start();
	void stop();

public:
	GTS_CXX_NODISCARD("")
	std::string view_status() const;

private:
	void service(tcp_socket *sock, bool universal) const;

private:
	class basic_site
	{
		GTS_DISABLE_COPY_MOVE(basic_site)

	public:
		basic_site(tcp_server *q_ptr, asio::io_context &io, const site_info &info);
		virtual ~basic_site() = 0;

	public:
		GTS_CXX_NODISCARD("")
		virtual std::string view_status() const;

		virtual bool start(); //coro
		void stop();

	protected:
		tcp_server *q_ptr;
		tcp::acceptor m_acceptor;
		tcp::endpoint m_endpoint;
		std::string m_addr;
		bool m_universal;
	};

	class tcp_site : public basic_site
	{
	public:
		using basic_site::basic_site;
		GTS_CXX_NODISCARD("") std::string view_status() const override;
		GTS_CXX_NODISCARD("") bool start() override; //coro
	};

#ifdef GTS_ENABLE_SSL
	class ssl_site : public basic_site
	{
	public:
		using basic_site::basic_site;
		GTS_CXX_NODISCARD("") std::string view_status() const override;
		GTS_CXX_NODISCARD("") bool start() override; //coro
	};
	bool m_no_ck_file = true;
#endif //GTS_ENABLE_SSL

private:
	typedef std::shared_ptr<basic_site>  site_ptr;
	std::map<std::string, site_ptr> m_sites;

	std::size_t m_coro_stack_size = 8 * 1024 * 1024;
	std::size_t m_buffer_size = 65536;
};

GTS_NAMESPACE_END


#endif //TCP_SERVER_H
