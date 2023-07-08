#include <gts/tcp_socket.h>
#include <gts/web.h>
#include <iostream>

namespace gts { namespace web { namespace business
{

class GTS_DECL_EXPORT plugin1_0
{
public:
	future_ptr init();
	void exit();
	void new_request(http::request &&request, http::response &&response, environments &&envs);
};

future_ptr plugin1_0::init()
{
	return make_future_ptr(std::async(std::launch::async,[]
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

	auto body = fmt::format("Ok!!!\npath = {}\n\n", request.path());

	for(auto &para : request.parameters())
		body += fmt::format("(p) {} = {}\n", para.first, para.second.to_string());
	body += "\n";

	for(auto &para : request.headers())
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
	future_ptr init();
	void exit();
	std::string view_status();

public:
	void new_request_0(http::response &&response);
	void new_request_1(http::response &&response);
};

future_ptr plugin1_1::init()
{
	return make_future_ptr(std::async(std::launch::async, []
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

std::string plugin1_1::view_status()
{
	return "web plugin: examples-plugin1: hello1\n";
}

void plugin1_1::new_request_0(http::response &&response)
{
//	response.write("plugin1-1::0");
	response.redirect("https://www.baidu.com");
}

void plugin1_1::new_request_1(http::response &&response)
{
	response.write("plugin1-1::1");
}

}}} //namespace gts::web::business

GTS_PLUGIN_REGISTRATION
{
	using namespace gts::http;
	using namespace gts::web;

	registration::class_<business::plugin1_0>("/")
			.init_method(&business::plugin1_0::init)
			.exit_method(&business::plugin1_0::exit)
			.new_request_method<GET>("plugin1", &business::plugin1_0::new_request);

	registration::class_<business::plugin1_1>("plugin1")
			.init_method(&business::plugin1_1::init)
			.exit_method(&business::plugin1_1::exit)
			.view_status_method(&business::plugin1_1::view_status)
			.new_request_method<GET>("/sub0/", &business::plugin1_1::new_request_0)
			.new_request_method<GET>("sub1", &business::plugin1_1::new_request_1);
}
