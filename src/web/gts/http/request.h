#ifndef GTS_HTTP_REQUEST_H
#define GTS_HTTP_REQUEST_H

#include <gts/http/types.h>

namespace gts {
class tcp_socket;

namespace http
{

class request_impl;

class GTSWEB_API request
{
	GTS_DISABLE_COPY(request)

public:
	request();
	request(request &&other);
	~request();

public:
	http::method method() const;
	std::string version() const;
	std::string path() const;

public:
	std::string parameters_string() const;
	const http::parameters &parameters() const;
	const http::headers &headers() const;
	const basic_cookies &cookies() const; //unrealized

public:
	std::string read_body(std::size_t size = 0);
	std::size_t read_body(void *buf, std::size_t size);

public:
	bool keep_alive() const;
	bool support_gzip() const;

public:
	request &operator=(request &&other);
	bool is_valid() const;

public:
	const tcp_socket &socket() const;
	tcp_socket &socket();

private:
	friend class web::http_parser;
	friend class web::session;
	request_impl *m_impl;
};

}} //namespace gts::http


#endif //GTS_HTTP_REQUEST_H
