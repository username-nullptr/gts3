
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

#include "connection.h"
#include <gts/web/config_key.h>
#include <gts/coro_for_asio.h>
#include <gts/settings.h>

using namespace std::chrono;

GTS_WEB_NAMESPACE_BEGIN

static std::atomic_long g_counter {0};

connection::connection(tcp_socket_ptr socket) :
	m_socket(std::move(socket)),
	m_timer(io_context()),
	m_task(m_socket)
{
	g_counter++;

	tcp::socket::send_buffer_size attr;
	m_socket->get_option(attr);
	m_ab_size = attr.value();

	m_asio_buffer = new char[m_ab_size]{0};
	m_parser = new http::parser(m_ab_size);

	do_recv_work();
}

connection::~connection()
{
	g_counter--;
	gts_log_debug("Session deleted ({}).", g_counter);

	m_task.cancel();
	m_timer.cancel();

	delete m_parser;
	delete[] m_asio_buffer;
}

void connection::cancel()
{
	m_socket->close(true);
}

static long g_idle_time_tv = 3000;
static long g_max_idle_time = 60000;
static long g_max_count = 1024;

void connection::init()
{
	g_idle_time_tv = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_IDLE_TIME_TV, g_idle_time_tv);
	g_max_idle_time = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_MAX_IDLE_TIME, g_max_idle_time);
	g_max_count = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_MAX_SESSION_COUNT, g_max_count);

	if( g_idle_time_tv < 1 )
	{
		gts_log_warning("Config: idle time threshold value setting error.");
		g_idle_time_tv = 1;
	}
	if( g_max_idle_time <= g_idle_time_tv )
	{
		gts_log_warning("Config: max idle time setting error.");
		g_max_idle_time = g_idle_time_tv + 1;
	}
	if( g_max_count < 1 )
	{
		gts_log_warning("Config: max connection count setting error.");
		g_max_count = 1;
	}

	gts_log_debug("Web: idle time threshold value: {}", g_idle_time_tv);
	gts_log_debug("Web: max idle time: {}", g_max_idle_time);
	gts_log_debug("Web: max connetion count: {}", g_max_count);
	task::init();
}

static std::set<connection*> g_timeout_set;

void connection::new_connection(tcp_socket_ptr socket)
{
	if( g_counter < g_max_count )
	{
		new connection(std::move(socket));
		return ;
	}
	gts_log_info("Connection resource exhaustion, attempts to reclaim idle connection resources.");

	auto it = g_timeout_set.begin();
	if( it != g_timeout_set.end() )
	{
		(*it)->cancel();
		g_timeout_set.erase(it);
		new connection(std::move(socket));
		return ;
	}
	gts_log_warning("No connection resources are available and the server is overloaded.");

	asio::error_code error;
	socket->non_blocking(false, error);
	socket->write_some("HTTP/1.0 503 Service Unavailable\r\n"
					   "content-length: 0\r\n"
					   "connection: close\r\n"
					   "\r\n", error);
	socket->close(true);
}

std::string connection::view_status()
{
	return fmt::format("web plugin:\n"
					   "  connection count: {} / {}\n\n", g_counter, g_max_count);
}

void connection::do_recv_work()
{
	for(;;)
	{
		if( not m_socket->is_open() )
			return delete_later(this);

		start_coroutine([&]{
			do_timer_work();
		});
		asio::error_code error;
		auto size = m_socket->coro_await_read_some(m_asio_buffer, m_ab_size, error);

		m_timer.cancel();
		g_timeout_set.erase(this);

		if( error or size == 0 )
			return delete_later(this);
		else if( not m_parser->load(std::string(m_asio_buffer, size)) )
			continue;

		auto context = m_parser->get_request(m_socket);
		assert(context);

		if( not context->is_valid() )
		{
			m_socket->write_some("HTTP/1.1 400 Bad Request\r\n"
								 "content-length: 0\r\n"
								 "connection: close\r\n"
								 "\r\n");
			m_socket->close(true);
			return delete_later(this);
		}
		coro_await([&]
		{
			if( not m_task.run(context) )
				delete_later(this);
		},
		0x7FFFFF);
	}
}

void connection::do_timer_work()
{
	asio::error_code was_cancel;

	coro_await_timer(m_timer, std::chrono::milliseconds(g_idle_time_tv), was_cancel);
	if( was_cancel )
		return ;

	gts_log_debug("Session enters the idle state. (client: {})", m_socket->remote_endpoint());
	g_timeout_set.emplace(this);

	coro_await_timer(m_timer, std::chrono::milliseconds(g_max_idle_time), was_cancel);
	if( was_cancel )
		return ;

	gts_log_debug("The life cycle of connection ends. (client: {})", m_socket->remote_endpoint());
	g_timeout_set.erase(this);
	cancel();
}

GTS_WEB_NAMESPACE_END
