#ifndef GTS_WEB_SOCKET_H
#define GTS_WEB_SOCKET_H

#include <gts/web/types.h>
#include <gts/tcp_socket.h>

#ifdef GTS_ENABLE_SSL
# include <gts/ssl_socket.h>
#endif //ssl

// Not Implemented
namespace gts { namespace web
{

class GTS_WEB_API socket
{
	GTS_DISABLE_COPY_MOVE(socket)
	using duration = std::chrono::milliseconds;

public:
	explicit socket(tcp_socket_ptr sock);
	virtual ~socket();

public:


protected:
	tcp_socket_ptr m_sock;
};

using socket_ptr = std::shared_ptr<socket>;

} //namespace web

using websocket = web::socket;

using websocket_ptr = web::socket_ptr;

} //namespace gts


#endif //GTS_WEB_SOCKET_H
