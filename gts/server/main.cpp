#include "application.h"
#include "server/tcp_server.h"
#include "server/cmdline_server.h"

int main(int argc, const char *argv[])
{
	gts::applictaion app(argc, argv);
	app.extension_init();

	gts::tcp_server tcp_server;
	tcp_server.start();

	gts::cmdline_server cmdline_server(tcp_server);
	cmdline_server.start();

	int res = app.exec();
	cmdline_server.stop();
	tcp_server.stop();

	app.extension_exit();
	return res;
}
