#ifndef SESSION_H
#define SESSION_H

#include "session/http_parser.h"
#include "service/task.h"

namespace gts { namespace web
{

class GTS_DECL_HIDDEN session
{
	GTS_DISABLE_COPY_MOVE(session)
	session(tcp_socket_ptr socket);

public:
	~session();
	void cancel();

public:
	static void init();
	static void exit();
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
	task m_task;
};

}} //namespace gts::web


#endif //SESSION_H
