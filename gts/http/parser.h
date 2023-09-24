#ifndef GTS_HTTP_PARSER_H
#define GTS_HTTP_PARSER_H

#include <gts/http/service_context.h>
#include <gts/tcp_socket.h>

namespace gts { namespace http
{

class parser_impl;

class GTS_HTTP_API parser
{
	GTS_DISABLE_COPY_MOVE(parser)

public:
	explicit parser(int buf_size = 65536);
	~parser();

public:
	bool write(const std::string &data);
	service_context_ptr get_request(tcp_socket_ptr socket);

private:
	parser_impl *m_impl;
};

}} //namespace gts::http


#endif //GTS_HTTP_PARSER_H
