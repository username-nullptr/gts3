#ifndef SESSION_H
#define SESSION_H

#include "global.h"

namespace gts { namespace web
{

namespace http{
class parser;
}

class co_task;

class GTS_DECL_HIDDEN session
{
	DISABLE_COPY(session)

public:
	session(std::shared_ptr<tcp::socket> socket);
	~session();

public:
	static void init();
	static void exit();

public:
	static std::size_t count();

private:
	void do_recv();
	void time_out(const asio::error_code &was_cancel);

private:
	std::shared_ptr<tcp::socket> m_socket;

private:
	asio::steady_timer m_timer;
	char *m_asio_buffer = nullptr;
	std::size_t m_ab_size = 65536;

private:
	http::parser *m_parsing = nullptr;
	co_task *m_task = nullptr;
};

}} //namespace gts::web


#endif //SESSION_H
