#ifndef CO_TASK_H
#define CO_TASK_H

#include "global.h"
#include <functional>

namespace gts { namespace web
{

namespace http {
class request;
}

class GTS_DECL_HIDDEN co_task
{
	DISABLE_COPY(co_task)

public:
	co_task(std::shared_ptr<tcp::socket> socket, std::shared_ptr<http::request> request);
	~co_task();

public:
	static void init();
	static void exit();

public:
	void start();

public:
	void async_wait_next(std::function<void()> call_back);
	void cancel();

private:
	void run();

private:
	std::shared_ptr<tcp::socket> m_socket;
	std::shared_ptr<http::request> m_request;
	std::function<void()> m_call_back;
};

}} //namespace gts::web


#endif //CO_TASK_H
