#ifndef GTS_WEBSOCKET_H
#define GTS_WEBSOCKET_H

#include <gts/web_global.h>
#include <gts/tcp_socket.h>

#ifdef GTS_ENABLE_SSL
# include <gts/ssl_socket.h>
#endif //ssl

// Not Implemented
namespace gts
{

class GTSWEB_API websocket
{
	GTS_DISABLE_COPY_MOVE(websocket)
	using duration = std::chrono::milliseconds;

public:
	explicit websocket(tcp_socket_ptr sock);
	virtual ~websocket();



protected:
	tcp_socket_ptr m_sock;
};

using websocket_ptr = std::shared_ptr<websocket>;

} //namespace gts


#endif //GTS_WEBSOCKET_H
