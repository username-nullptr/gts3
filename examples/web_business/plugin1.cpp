#include <gts/tcp_socket.h>
#include <gts/web.h>

#include <iostream>
#include <future>

namespace gts { namespace web { namespace business
{

class GTS_DECL_EXPORT plugin1_0
{
public:
	std::shared_ptr<std::future<void>> init();
	void exit();
	void new_request(http::request &&request, http::response &&response, environments &&envs);
};

std::shared_ptr<std::future<void>> plugin1_0::init()
{
	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
	{
		for(int i=0; i<10; i++)
		{
			std::cerr << "plugin1-0: init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(150));
		}
	}));
}

void plugin1_0::exit()
{
	std::cerr << "plugin1-0: exit task ..." << std::endl;
}

void plugin1_0::new_request(http::request &&request, http::response &&response, environments &&envs)
{
	std::cerr << std::endl;

	auto body = fmt::format("Ok!!!\npath = {}\n\n", request.path);

	for(auto &para : request.parameters)
		body += fmt::format("(p) {} = {}\n", para.first, para.second.to_string());
	body += "\n";

	for(auto &para : request.headers)
		body += fmt::format("(h) {} = {}\n", para.first, para.second);
	body += "\n";

	for(auto &para : envs)
		body += fmt::format("(e) {} = {}\n", para.first, para.second);
	body += "\n";

	body.erase(body.size() - 2);
	response.write(body);
}

class GTS_DECL_EXPORT plugin1_1
{
public:
	std::shared_ptr<std::future<void>> init();
	void exit();
	void new_request_0(http::response &&response);
	void new_request_1(http::response &&response);
};

std::shared_ptr<std::future<void>> plugin1_1::init()
{
	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
	{
		for(int i=0; i<4; i++)
		{
			std::cerr << "plugin1-1: init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(550));
		}
	}));
}

void plugin1_1::exit()
{
	std::cerr << "plugin1-1: exit task ..." << std::endl;
}

void plugin1_1::new_request_0(http::response &&response)
{
	response.write("plugin1-1::0");
}

void plugin1_1::new_request_1(http::response &&response)
{
	response.write("plugin1-1::1");
}

static std::string view_status()
{
	return "web plugin: examples-plugin1: hello1\n";
}

}}} //namespace gts::web::business

RTTR_PLUGIN_REGISTRATION
{
	using namespace gts::http;
	using namespace gts::web;

	registration().view_status_method(business::view_status);

	registration::class_<business::plugin1_0>("/")
			.init_method(&business::plugin1_0::init)
			.exit_method(&business::plugin1_0::exit)
			.new_request_method<GET>("plugin1", &business::plugin1_0::new_request);

	registration::class_<business::plugin1_1>("plugin1")
			.init_method(&business::plugin1_1::init)
			.exit_method(&business::plugin1_1::exit)
			.new_request_method<GET>("/sub0/", &business::plugin1_1::new_request_0)
			.new_request_method<GET>("sub1", &business::plugin1_1::new_request_1);
}
