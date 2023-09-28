#include "service_context.h"
#include "detail/request_impl.h"

namespace gts { namespace http
{

class GTS_DECL_HIDDEN service_context_impl
{
	GTS_DISABLE_COPY_MOVE(service_context_impl)

public:
	service_context_impl(http::request *request) :
		m_requst(request), m_response(*request)
	{
		assert(request);
		m_requst->m_impl->m_response = &m_response;
	}

	~service_context_impl() {
		delete_later(m_requst);
	}

public:
	request *m_requst;
	response m_response;
};

/*-------------------------------------------------------------------------------------------------------------------------------------*/

service_context::service_context(http::request *request) :
	m_impl(new service_context_impl(request))
{

}

service_context::~service_context()
{
	delete m_impl;
}

request &service_context::request()
{
	return *m_impl->m_requst;
}

response &service_context::response()
{
	return m_impl->m_response;
}

tcp_socket &service_context::socket()
{
	return m_impl->m_requst->m_impl->socket();
}

bool service_context::is_valid() const
{
	return m_impl->m_requst->is_valid();
}

}} //namespace gts::http
