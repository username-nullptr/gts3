#include "application.h"
#include "server/tcp_server.h"
#include "server/cmdline_server.h"
//#include "gts/args_parser.h"
//#include <iostream>

int main(int argc, const char *argv[])
{
//	gts::cmdline::args_parser parser("gts3 help:");
//	auto hash = parser.add_flag("-a,--aAA", "aaaaaa", "aa")
//					.add_flag("-b", "bbbbbb")
//					.add_flag("-c", "ccccc")
//					.add_group("-d", "ddddd")
//					.set_help_extension("ahaskfe")
//					.parsing(argc, argv);

//	for(auto &pair : hash)
//		std::cout << pair.first << "  " << pair.second << std::endl;

	gts::applictaion app(argc, argv);

	gts::tcp_server tcp_server;
	tcp_server.start();

	gts::cmdline_server cmdline_server(tcp_server);
	cmdline_server.start();

	int res = app.exec();
	cmdline_server.stop();
	tcp_server.stop();
	return res;
}
