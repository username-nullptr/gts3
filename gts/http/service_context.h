#ifndef GTS_HTTP_SERVICE_CONTEXT_H
#define GTS_HTTP_SERVICE_CONTEXT_H

#include <gts/http/request.h>
#include <gts/http/response.h>

namespace gts { namespace http
{

class service_context_impl;

class GTS_HTTP_API service_context
{
	GTS_DISABLE_COPY_MOVE(service_context)

public:
	explicit service_context(http::request *request);
	~service_context();

public:
	http::request &request();
	http::response &response();

public:
	tcp_socket &socket();
	bool is_valid() const;

private:
	service_context_impl *m_impl;
};
using service_context_ptr = std::shared_ptr<service_context>;

}} //namespace gts::http


#endif //GTS_HTTP_SERVICE_CONTEXT_H
