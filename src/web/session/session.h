#ifndef SESSION_H
#define SESSION_H

#include "session/http_parser.h"
#include "service/task.h"

namespace gts { namespace web
{

class GTS_DECL_HIDDEN basic_session
{
protected: virtual ~basic_session() = 0;
public :virtual void cancel() = 0;
};

template <class asio_socket>
class GTS_DECL_HIDDEN session : public basic_session
{
	DISABLE_COPY(session)
	typedef gts::socket<asio_socket>     tcp_socket;
	typedef std::shared_ptr<tcp_socket>  tcp_socket_ptr;
	session(tcp_socket_ptr socket);

public:
	~session() override;
	void cancel() override;

public:
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
	http_parser *m_parser = nullptr;
	task<asio_socket> m_task;
};

class GTS_DECL_HIDDEN session_config
{
public:
	static long idle_time_tv;
	static long max_idle_time;
	static long max_count;

public:
	static std::atomic_long counter;
	static std::set<basic_session*> timeout_set;

public:
	static void init();
	static void exit();
	static std::string view_status();
};

template <class asio_socket>
session<asio_socket>::session(tcp_socket_ptr socket) :
	m_socket(std::move(socket)),
	m_timer(io_context()),
	m_task(m_socket)
{
	session_config::counter++;

	tcp::socket::send_buffer_size attr;
	m_socket->get_option(attr);
	m_ab_size = attr.value();

	m_asio_buffer = new char[m_ab_size]{0};
	m_parser = new http_parser(m_ab_size);

	do_recv();
}

template <class asio_socket>
session<asio_socket>::~session()
{
	typedef session_config  sc;

	sc::counter--;
	log_debug("Session deleted ({}).", sc::counter);

	m_task.cancel();
	m_timer.cancel();

	delete m_parser;
	delete[] m_asio_buffer;
}

template <class asio_socket>
void session<asio_socket>::cancel()
{
	m_socket->shutdown(tcp::socket::shutdown_both);
	m_socket->close();
}

template <class asio_socket>
void session<asio_socket>::new_connection(tcp_socket_ptr socket)
{
	typedef session_config  sc;
	asio::post(io_context(), [socket]
	{
		if( sc::counter < sc::max_count )
		{
			new session(std::move(socket));
			return ;
		}
		log_info("Connection resource exhaustion, attempts to reclaim idle connection resources.");

		auto it = sc::timeout_set.begin();
		if( it != sc::timeout_set.end() )
		{
			(*it)->cancel();
			sc::timeout_set.erase(it);
			new session<asio_socket>(std::move(socket));
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

template <class asio_socket>
void session<asio_socket>::do_recv()
{
	typedef session_config  sc;
	if( not m_socket->is_open() )
		return delete_later(this);

	m_timer.expires_after(std::chrono::milliseconds(sc::idle_time_tv));
	m_timer.async_wait(std::bind(&session<asio_socket>::time_out_allow_preemptionx, this, std::placeholders::_1));

	m_socket->async_read_some(
				asio::buffer(m_asio_buffer, m_ab_size),
				[this](const asio::error_code &error, std::size_t size)
	{
		m_timer.cancel();
		sc::timeout_set.erase(this);

		if( error or size == 0 )
			return delete_later(this);

		auto request = m_parser->write(std::string(m_asio_buffer, size));
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

		m_task.async_wait_next([this]{
			do_recv();
		});
		m_task.start(std::move(request));
	});
}

template <class asio_socket>
void session<asio_socket>::time_out_allow_preemptionx(const asio::error_code &was_cancel)
{
	typedef session_config  sc;
	if( was_cancel )
		return ;

	log_debug("Session enters the idle state. (client: {})", m_socket->remote_endpoint());
	sc::timeout_set.emplace(this);

	m_timer.expires_after(std::chrono::milliseconds(sc::max_idle_time));
	m_timer.async_wait(std::bind(&session<asio_socket>::time_out_destory, this, std::placeholders::_1));
}

template<class asio_socket>
void session<asio_socket>::time_out_destory(const asio::error_code &was_cancel)
{
	if( was_cancel )
		return ;

	log_debug("The life cycle of session ends. (client: {})", m_socket->remote_endpoint());
	session_config::timeout_set.erase(this);
	cancel();
}

}} //namespace gts::web


#endif //SESSION_H
