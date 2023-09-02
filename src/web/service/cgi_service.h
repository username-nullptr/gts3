#ifndef CGI_SERVICE_H
#define CGI_SERVICE_H

#include "process.h"
#include <condition_variable>
#include <atomic>
#include <mutex>

namespace gts { namespace web
{

#define _BUF_SIZE  65536

class service_io;

class GTS_DECL_HIDDEN cgi_service
{
	GTS_DISABLE_COPY_MOVE(cgi_service)

public:
	explicit cgi_service(service_io &sio);
	~cgi_service();

public:
	static void init();
	bool call();

private:
	void async_write_socket(const char *buf, std::size_t buf_size);
	void async_read_socket();

private:
	void async_write_cgi(const char *buf, std::size_t buf_size);
	void async_read_cgi();

private:
	std::string replace_http_to_env(const std::string &str);

private:
	service_io &m_sio;
	process m_cgi;

private:
	char m_cgi_read_buf[_BUF_SIZE];
	std::size_t m_content_length = 0;
	std::size_t m_tcp_buf_size = _BUF_SIZE;
	char *m_sock_read_buf = nullptr;

private:
	std::atomic_int m_counter {0};
	std::condition_variable m_condition;
	std::mutex m_mutex;
};

}} //namespace gts::web


#endif //CGI_SERVICE_H
