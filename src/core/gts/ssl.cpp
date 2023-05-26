#ifdef GTS_ENABLE_SSL

#include "ssl.h"
#include <asio.hpp>
#include <asio/ssl.hpp>

namespace gts
{

static ssl::context g_ssl_context { ssl::context::sslv23_server };

ssl::context &asio_ssl_context()
{
	return g_ssl_context;
}

} //namespace gts

#endif //ssl
