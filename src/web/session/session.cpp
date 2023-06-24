#include "session.h"
#include "service/task.h"

#include "gts/web/config_key.h"
#include "gts/settings.h"
#include "gts/log.h"

using namespace std::chrono;

namespace gts { namespace web
{

static std::atomic_long g_counter {0};

session::session(tcp_socket_ptr socket) :
	m_socket(std::move(socket)),
	m_timer(io_context()),
	m_task(m_socket)
{
	g_counter++;

	tcp::socket::send_buffer_size attr;
	m_socket->get_option(attr);
	m_ab_size = attr.value();

	m_asio_buffer = new char[m_ab_size]{0};
	m_parser = new http_parser(m_ab_size);

	do_recv();
}

session::~session()
{
	g_counter--;
	log_debug("Session deleted ({}).", g_counter);

	m_task.cancel();
	m_timer.cancel();

	delete m_parser;
	delete[] m_asio_buffer;
}

void session::cancel()
{
	m_socket->close(true);
}

static long g_idle_time_tv = 3000;
static long g_max_idle_time = 60000;
static long g_max_count = 1024;

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
	if( g_max_idle_time <= g_idle_time_tv )
	{
		log_warning("Config: max idle time setting error.");
		g_max_idle_time = g_idle_time_tv + 1;
	}
	if( g_max_count < 1 )
	{
		log_warning("Config: max session count setting error.");
		g_max_count = 1;
	}

	log_debug("Web: idle time threshold value: {}", g_idle_time_tv);
	log_debug("Web: max idle time: {}", g_max_idle_time);
	log_debug("Web: max connetion count: {}", g_max_count);
	task::init();
}

void session::exit()
{
	task::exit();
}

static std::set<session*> g_timeout_set;

void session::new_connection(tcp_socket_ptr socket)
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
		(*it)->cancel();
		g_timeout_set.erase(it);
		new session(std::move(socket));
		return ;
	}
	log_warning("No connection resources are available and the server is overloaded.");

	asio::error_code error;
	socket->non_blocking(false, error);
	socket->write_some("HTTP/1.0 503 Service Unavailable\r\n"
					   "content-length: 0\r\n"
					   "connection: close\r\n"
					   "\r\n", error);
	socket->close(true);
}

std::string session::view_status()
{
	auto result = fmt::format("web plugin:\n"
							  "  session count: {} / {}\n", g_counter, g_max_count);
	auto tmp = task::view_status();

	if( not tmp.empty() )
	{
		if( tmp[0] != '\n' )
			tmp = "\n" + tmp;
		result += tmp;
	}
	return result;
}

void session::do_recv()
{
	if( not m_socket->is_open() )
		return delete_later(this);

	m_timer.expires_after(std::chrono::milliseconds(g_idle_time_tv));
	m_timer.async_wait(std::bind(&session::time_out_allow_preemptionx, this, std::placeholders::_1));

	m_socket->async_read_some(m_asio_buffer, m_ab_size,
				[this](const asio::error_code &error, std::size_t size)
	{
		m_timer.cancel();
		g_timeout_set.erase(this);

		if( error or size == 0 )
			return delete_later(this);

		auto request = m_parser->write(std::string(m_asio_buffer, size));
		if( request == nullptr )
			return do_recv();

		else if( not request->is_valid() )
		{
			m_socket->write_some("HTTP/1.1 400 Bad Request\r\n"
								 "content-length: 0\r\n"
								 "connection: close\r\n"
								 "\r\n");
			m_socket->close(true);
			return delete_later(this);
		}

		m_task.async_wait_next([this]{
			do_recv();
		});
		m_task.start(std::move(request));
	});
}

void session::time_out_allow_preemptionx(const asio::error_code &was_cancel)
{
	if( was_cancel )
		return ;

	log_debug("Session enters the idle state. (client: {})", m_socket->remote_endpoint());
	g_timeout_set.emplace(this);

	m_timer.expires_after(std::chrono::milliseconds(g_max_idle_time));
	m_timer.async_wait(std::bind(&session::time_out_destory, this, std::placeholders::_1));
}

void session::time_out_destory(const asio::error_code &was_cancel)
{
	if( was_cancel )
		return ;

	log_debug("The life cycle of session ends. (client: {})", m_socket->remote_endpoint());
	g_timeout_set.erase(this);
	cancel();
}

}} //namespace gts::web
