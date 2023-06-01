#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "gts/web_global.h"

namespace gts { namespace http {
struct request;
}} //namespace gts::http

namespace gts { namespace web
{

class GTS_DECL_HIDDEN http_parser
{
	DISABLE_COPY(http_parser)

public:
	explicit http_parser(int buf_size = 65536);
	~http_parser();

public:
	std::shared_ptr<http::request> write(const std::string &data);

private:
	bool state_handler_waiting_request(const std::string &line_buf);
	http::request *state_handler_reading_headers(const std::string &line_buf);
	http::request *next_request_ready();
	void reset();

private:
	enum state
	{
		waiting_request,
		reading_headers,
	}
	m_state = waiting_request;

private:
	std::string m_buffer;
	http::request *m_cache = nullptr;
};

}} //namespace gts::web


#endif //HTTP_PARSER_H
