#ifndef CMDLINE_SERVER_H
#define CMDLINE_SERVER_H

#include "gts/global.h"

namespace gts
{

namespace cmdline {
class interaction;
}

class tcp_server;

class cmdline_server
{
	GTS_DISABLE_COPY_MOVE(cmdline_server)

public:
	cmdline_server(tcp_server &tcp_server);
	~cmdline_server();

public:
	void start();
	void stop();

private:
	cmdline::interaction *m_interaction;
	tcp_server &m_tcp_server;
	char *m_buf;
};

} //namespace gts


#endif //CMDLINE_SERVER_H
