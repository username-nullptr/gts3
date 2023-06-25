#include <gts/registration.h>
#include <gts/web.h>

#include <iostream>
#include <future>

namespace gts { namespace web { namespace business
{
static std::shared_ptr<std::future<void>> init()
{
	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
	{
		for(int i=0; i<2; i++)
		{
			std::cerr << "plugin2: global init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}));
}

static void exit()
{
	std::cerr << "plugin2: global exit task ..." << std::endl;
}

static std::string view_status()
{
	return "web plugin: examples-plugin2: hello2\n";
}

void new_request_0(http::response &&response)
{
	response.write("plugin2-global");
}

class GTS_DECL_EXPORT plugin2
{
public:
	std::shared_ptr<std::future<void>> init();
	void exit();
	void new_request_0(http::response &&response);
	void new_request_1(http::response &&response);
};

std::shared_ptr<std::future<void>> plugin2::init()
{
	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
	{
		for(int i=0; i<8; i++)
		{
			std::cerr << "plugin2: init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(120));
		}
	}));
}

void plugin2::exit()
{
	std::cerr << "plugin2: exit task ..." << std::endl;
}

void plugin2::new_request_0(http::response &&response)
{
	response.write("plugin2-0");
}

void plugin2::new_request_1(http::response &&response)
{
	response.write("plugin2-1");
}

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

}}} //namespace gts::web::business

RTTR_PLUGIN_REGISTRATION
{
	using namespace gts::http;
	using namespace gts::web;

	registration()
			.init_method(business::init)
			.exit_method(business::exit)
			.view_status_method(business::view_status)
			.new_request_method<GET>("plugin2", business::new_request_0);

	registration::class_<business::plugin2>("plugin2/sub")
			.init_method(&business::plugin2::init)
			.exit_method(&business::plugin2::exit)
			.new_request_method<GET>("subsub", &business::plugin2::new_request_0)
			.new_request_method<GET>("subsub/test", &business::plugin2::new_request_1);

#if 0
	for(auto &pair : gts::get_site_infos())
	{
		auto &info = pair.second;
		if( not info.universal )
			gts::registration(business::intercept, info.port);
	}
#endif
}
