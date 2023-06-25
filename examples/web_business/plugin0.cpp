#include <gts/tcp_socket.h>
#include <gts/web.h>

#include <iostream>
#include <future>

using namespace asio::ip;

namespace gts { namespace web { namespace business
{

static std::shared_ptr<std::future<void>> init()
{
	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
	{
		for(int i=0; i<3; i++)
		{
			std::cerr << "plugin0: init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}));
}

//staic std::shared_ptr<std::future<void>> exit()
//{
//	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
//	{
//		for(int i=0; i<10; i++)
//		{
//			std::cerr << "plugin0: exit task ..." << std::endl;
//			std::this_thread::sleep_for(std::chrono::milliseconds(100));
//		}
//	}));
//}

static void exit()
{
	std::cerr << "plugin0: exit task ..." << std::endl;
}

static std::string view_status()
{
	return "web plugin: examples-plugin0: hello0\n";
}

static void new_request_0(http::response &&response)
{
	response.write("hello world");
}

static void new_request_1(http::response &&response)
{
	response.write("HELLO WORLD");
}

}}} //namespace gts::web::business

RTTR_PLUGIN_REGISTRATION
{
	using namespace gts::http;
	using namespace gts::web;

	registration()
			.init_method(business::init)
			.exit_method(business::exit)
			.view_status_method(business::view_status)
			.new_request_method<GET>("plugin0", business::new_request_0)
			.new_request_method<GET>("plugin0/sub", business::new_request_1);
}
