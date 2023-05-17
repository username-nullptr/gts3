#include "session.h"
#include "settings.h"
#include "gts/web_config_key.h"
#include "gts_log.h"

#include "http/parser.h"
#include "http/request.h"
#include "service/co_task.h"

#include <functional>
#include <chrono>

using namespace std::chrono;

namespace gts { namespace web
{

static int g_recv_timeout = 10000;

static std::atomic<std::size_t> g_counter {0};

session::session(std::shared_ptr<tcp::socket> socket) :
	m_socket(socket),
	m_timer(io_context(), g_recv_timeout)
{
	g_counter++;

	tcp::socket::send_buffer_size attr;
	socket->get_option(attr);

	m_ab_size = attr.value();
	m_asio_buffer = new char[m_ab_size]{0};
	m_parsing = new http::parser(m_ab_size);

	do_recv();
}

session::~session()
{
	log_debug("session deleted.");
	g_counter--;

	if( m_task )
		m_task->cancel();
	m_timer.cancel();

	delete m_parsing;
	delete[] m_asio_buffer;
}

void session::init()
{
	g_recv_timeout = settings::global_instance().read<int>(SINI_GROUP_WEB, SINI_WEB_NET_TIMEOUT, g_recv_timeout);
	if( g_recv_timeout < 1 )
	{
		log_warning("Config: Request wait time setting error.");
		g_recv_timeout = 1;
	}

	log_debug("Web: recv timeout: {}", g_recv_timeout);
	co_task::init();
}

void session::exit()
{
	co_task::exit();
}

std::size_t session::count()
{
	return g_counter;
}

void session::do_recv()
{
	if( not m_socket->is_open() )
		return delete_later(this);

	m_timer = asio::steady_timer(io_context(), std::chrono::milliseconds(g_recv_timeout));
	m_timer.async_wait(std::bind(&session::time_out, this, std::placeholders::_1));

	m_socket->async_read_some(
				asio::buffer(m_asio_buffer, m_ab_size),
				[this](const asio::error_code &error, std::size_t size)
	{
		m_timer.cancel();
		if( error or size == 0 )
			return delete_later(this);

		auto request = m_parsing->write(std::string(m_asio_buffer, size));
		if( request == nullptr )
			return do_recv();
		else if( not request->is_valid() )
		{
			m_socket->write_some(asio::buffer("HTTP/1.1 400 Bad Request\r\n"
											  "content-length: 0\r\n"
											  "connection: close\r\n"
											  "\r\n"));
			m_socket->shutdown(tcp::socket::shutdown_both);
			m_socket->close();
			return delete_later(this);
		}

		m_task = new co_task(m_socket, request);
		m_task->async_wait_next([this]()
		{
			m_task = nullptr;
			do_recv();
		});
		m_task->start();
	});
}

void session::time_out(const asio::error_code &was_cancel)
{
	if( was_cancel )
		return ;
	log_info("Request wait time out. (client: {})", m_socket->remote_endpoint());

	m_socket->shutdown(tcp::socket::shutdown_both);
	m_socket->close();
}

}} //namespace gts::web
