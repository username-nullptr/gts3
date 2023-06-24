#ifndef TASK_H
#define TASK_H

#include "gts/http/request.h"
#include "gts/tcp_socket.h"
#include <functional>

namespace gts
{

namespace http {
struct request;
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
	static void exit();
	static std::string view_status();

public:
	void start(std::shared_ptr<http::request> request);
	void async_wait_next(std::function<void()> call_back);
	void cancel();

private:
	void run();

private:
	tcp_socket_ptr m_socket;
	std::shared_ptr<http::request> m_request;
	std::function<void()> m_call_back;
};

}} //namespace gts::web


#endif //TASK_H
