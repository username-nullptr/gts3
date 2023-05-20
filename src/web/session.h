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

private:
	session(std::shared_ptr<tcp::socket> socket);

public:
	~session();
	static void init();
	static void exit();

public:
	static void new_connection(std::shared_ptr<tcp::socket> socket);
	static std::string view_status();

private:
	void do_recv();
	void time_out(const asio::error_code &was_cancel);

private:
	void reuse(std::shared_ptr<tcp::socket> socket);
	void set();

private:
	std::shared_ptr<tcp::socket> m_socket;
	asio::steady_timer m_timer;
	bool m_etv = false;
	bool m_reuse = false;

private:
	char *m_asio_buffer = nullptr;
	std::size_t m_ab_size = 65536;

private:
	http::parser *m_parsing = nullptr;
	co_task *m_task = nullptr;
};

}} //namespace gts::web


#endif //SESSION_H
