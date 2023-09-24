#ifndef TASK_H
#define TASK_H

#include "gts/http/service_context.h"
#include "gts/tcp_socket.h"
#include <functional>

namespace gts
{

namespace http {
class request;
} //namespace http

namespace web
{

class GTS_DECL_HIDDEN task
{
	GTS_DISABLE_COPY_MOVE(task)

public:
	explicit task(tcp_socket_ptr socket);

public:
	static void init();

public:
	void start(http::service_context_ptr context);
	void async_wait_next(std::function<void(bool)> call_back);
	void cancel();

private:
	void run();

private:
	tcp_socket_ptr m_socket;
	http::service_context_ptr m_context;
	std::function<void(bool)> m_call_back;
};

}} //namespace gts::web


#endif //TASK_H
