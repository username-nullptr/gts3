#ifndef PARSER_H
#define PARSER_H

#include "global.h"

namespace gts { namespace web { namespace http
{

class request;

class GTS_DECL_HIDDEN parser
{
	DISABLE_COPY(parser)

public:
	explicit parser(int buf_size = 65536);
	~parser();

public:
	std::shared_ptr<request> write(const std::string &data);

private:
	void state_handler_waiting_request(const std::string &line_buf);
	request *state_handler_reading_headers(const std::string &line_buf);
	request *next_request_ready();
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
	request *m_cache = nullptr;
};

}}} //namespace gts::web::http


#endif //PARSER_H
