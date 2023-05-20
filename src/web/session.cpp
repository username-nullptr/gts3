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

static long g_idle_time_tv = 3000;

static long g_max_idle_time = 60000;

static long g_max_count = 1024;

static std::atomic_long g_counter {0};

static std::set<session*>  g_timeout_set;

session::session(std::shared_ptr<tcp::socket> socket) :
	m_socket(std::move(socket)),
	m_timer(io_context())
{
	g_counter++;
	set();
	do_recv();
}

session::~session()
{
	g_counter--;
	log_debug("Session deleted ({}).", g_counter);

	if( m_task )
		m_task->cancel();
	m_timer.cancel();

	delete m_parsing;
	delete[] m_asio_buffer;
}

void session::init()
{
	g_idle_time_tv = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_IDLE_TIME_TV, g_idle_time_tv);
	g_max_idle_time = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_MAX_IDLE_TIME, g_max_idle_time);
	g_max_count = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_MAX_SESSION_COUNT, g_max_count);

	if( g_idle_time_tv < 1 )
	{
		log_warning("Config: idle time threshold value setting error.");
		g_idle_time_tv = 1;
	}
	if( g_max_idle_time < 1 )
	{
		log_warning("Config: max idle time setting error.");
		g_max_idle_time = 1;
	}
	if( g_max_count < 1 )
	{
		log_warning("Config: max session count setting error.");
		g_max_count = 1;
	}

	log_debug("Web: idle time threshold value: {}", g_idle_time_tv);
	log_debug("Web: max idle time: {}", g_max_idle_time);
	log_debug("Web: max connetion count: {}", g_max_count);
	co_task::init();
}

void session::exit()
{
	co_task::exit();
}

void session::new_connection(std::shared_ptr<tcp::socket> socket)
{
	asio::post(io_context(), [socket]
	{
		if( g_counter < g_max_count )
		{
			new session(std::move(socket));
			return ;
		}
		log_info("Connection resource exhaustion, attempts to reclaim idle connection resources.");

		auto it = g_timeout_set.begin();
		if( it != g_timeout_set.end() )
		{
			log_info("Reclaim idle connection resources. ({})", (*it)->m_socket->remote_endpoint());
			(*it)->reuse(std::move(socket));
			g_timeout_set.erase(it);
			return ;
		}
		log_warning("No connection resources are available and the server is overloaded.");

		asio::error_code error;
		socket->write_some(asio::buffer("HTTP/1.0 413 Payload Too Large\r\n"
										"content-length: 0\r\n"
										"\r\n"
										), error);
		socket->shutdown(tcp::socket::shutdown_both);
		socket->close();
	});
}

std::string session::view_status()
{
	return fmt::format("session count: {} / {}\n", g_counter, g_max_count);
}

void session::do_recv()
{
	if( not m_socket->is_open() )
		return delete_later(this);

	m_timer.expires_after(std::chrono::milliseconds(g_idle_time_tv));
	m_timer.async_wait(std::bind(&session::time_out, this, std::placeholders::_1));

	m_socket->async_read_some(
				asio::buffer(m_asio_buffer, m_ab_size),
				[this](const asio::error_code &error, std::size_t size)
	{
		m_etv = false;
		m_timer.cancel();
		g_timeout_set.erase(this);

		if( error or size == 0 )
		{
			if( not m_reuse )
				return delete_later(this);
			m_reuse = false;
			return do_recv();
		}

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

	if( not m_etv )
	{
		log_debug("Session enters the idle state. (client: {})", m_socket->remote_endpoint());
		m_etv = true;
		g_timeout_set.emplace(this);
		m_timer.expires_after(std::chrono::milliseconds(g_max_idle_time));
	}
	else
	{
		log_debug("The life cycle of session ends. (client: {})", m_socket->remote_endpoint());
		g_timeout_set.erase(this);
		delete_later(this);
	}
}

void session::reuse(std::shared_ptr<tcp::socket> socket)
{
	log_debug("Sesson reset. (old: {} , new: {})", m_socket->remote_endpoint(), socket->remote_endpoint());
	m_reuse = true;

	m_timer.cancel();
	m_socket->cancel();

	m_socket->shutdown(tcp::socket::shutdown_both);
	m_socket->close();
	m_socket = std::move(socket);

	delete m_parsing;
	delete[] m_asio_buffer;
	set();
}

void session::set()
{
	tcp::socket::send_buffer_size attr;
	m_socket->get_option(attr);

	m_ab_size = attr.value();
	m_asio_buffer = new char[m_ab_size]{0};
	m_parsing = new http::parser(m_ab_size);
}

}} //namespace gts::web
