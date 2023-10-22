#include <gts/web/registration.h>
#include <gts/http/formatter.h>
#include <iostream>

namespace gts { namespace web { namespace business
{

using namespace http;

GTS_PLUGIN_ASYNC_INIT()
{
	return make_future_ptr(std::async(std::launch::async,[]
	{
		for(int i=0; i<2; i++)
		{
			std::cerr << "plugin2: global init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}));
}

GTS_PLUGIN_EXIT(){
	std::cerr << "plugin2: global exit task ..." << std::endl;
}

GTS_PLUGIN_VIEW_STATUS(){
	return "web plugin: examples-plugin2: hello2\n";
}

GTS_PLUGIN_NEW_WEBSOCKET_CONNECTION("websocket_test", GET|PUT|POST, web::socket_ptr socket)
{
	gts_log_error("000: websocket_test ..............");
}

class GTS_DECL_EXPORT plugin2
{
public:
	future_ptr init()
	{
		return make_future_ptr(std::async(std::launch::async,[]
		{
			for(int i=0; i<8; i++)
			{
				std::cerr << "plugin2: init task ..." << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(120));
			}
		}));
	}

	void exit()
	{
		std::cerr << "plugin2: exit task ..." << std::endl;
	}

	bool request_filter(http::request &request)
	{
		std::cerr << fmt::format("plugin2: request_filter: '{} ({:s})'.", request.path(), request.method()) << std::endl;
		return false;
	}

	void new_request_0(http::response &response)
	{
		response.write("plugin2-0");
	}

	void new_request_1(http::response &response)
	{
//		response.write("plugin2-1");
		response.write_file("button.html");
	}
};

#if 0
static void intercept(tcp_socket_ptr socket)
{
	asio::error_code error;
	socket->write_some("HTTP/1.1 200 OK\r\n"
					   "content-length: 22\r\n"
					   "content-type: text/plain; charset=utf-8\r\n"
					   "connection: close\r\n"
					   "\r\n"
					   "plugin2: intercept ..."
					   , error);
	socket->close();
}
#endif

GTS_PLUGIN_REGISTRATION
{
	using namespace gts::http;
	using namespace gts::web;

	registration::class_<plugin2>("plugin2/sub")
			.init_method(&plugin2::init)
			.exit_method(&plugin2::exit)
			.filter_method("/", &plugin2::request_filter)
			.request_handle_method<GET>("subsub", &plugin2::new_request_0)
			.request_handle_method<GET>("subsub/test", &plugin2::new_request_1);

#if 0
	for(auto &pair : gts::get_site_infos())
	{
		auto &info = pair.second;
		if( not info.universal )
			gts::registration(intercept, info.port);
	}
#endif
}

}}} //namespace gts::web::business
