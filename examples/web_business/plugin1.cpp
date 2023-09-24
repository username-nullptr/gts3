#include <gts/web/registration.h>
#include <iostream>

namespace gts { namespace web { namespace business
{

class GTS_DECL_EXPORT plugin1_0
{
public:
	future_ptr init()
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

	void exit()
	{
		std::cerr << "plugin1-0: exit task ..." << std::endl;
	}

	void new_request(http::request &req, http::response &res, const environments &envs)
	{
		auto session = req.session();
		session->set_attribute("hello", "|** http session test.");

		res.set_cookie("hello", http::cookie("world").set_http_only(true));
		std::cerr << std::endl;

		auto body = fmt::format("Ok!!!\npath = {}\n\n", req.path());

		for(auto &para : req.parameters())
			body += fmt::format("(p) {} = {}\n", para.first, para.second);
		body += "\n";

		for(auto &para : req.headers())
			body += fmt::format("(h) {} = {}\n", para.first, para.second);
		body += "\n";

		for(auto &para : envs)
			body += fmt::format("(e) {} = {}\n", para.first, para.second);
		body += "\n";

		body.erase(body.size() - 2);
		res.write(body);
	}
};

class GTS_DECL_EXPORT plugin1_1
{
public:
	future_ptr init()
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

	void exit()
	{
		std::cerr << "plugin1-1: exit task ..." << std::endl;
	}

	std::string view_status()
	{
		return "web plugin: examples-plugin1: hello1\n";
	}

public:
	void new_request_0(http::response &response)
	{
//		response.write("plugin1-1::0");
//		response.redirect("https://gitee.com/jin-xiaoqiang/gts3.git");
		response.redirect("https://github.com/username-nullptr/gts3.git");
	}

	void new_request_1(http::response &response, http::request &request)
	{
		for(auto &pair : request.cookies())
			gts_log_info("cookie: [ {} ]", pair);

		auto session = request.session(false);
		if( session and session->is_valid() )
			gts_log_info("session attribute: [ hello - {} ]", session->attribute_or("hello").to_string());

		response.write("plugin1-1::1");
	}
};

GTS_PLUGIN_REGISTRATION
{
	using namespace gts::http;
	using namespace gts::web;

	registration::class_<plugin1_0>("/")
			.init_method(&plugin1_0::init)
			.exit_method(&plugin1_0::exit)
			.request_handle_method<GET>("plugin1", &plugin1_0::new_request);

	registration::class_<plugin1_1>("plugin1")
			.init_method(&plugin1_1::init)
			.exit_method(&plugin1_1::exit)
			.view_status_method(&plugin1_1::view_status)
			.request_handle_method<GET>("/sub0/", &plugin1_1::new_request_0)
			.request_handle_method<GET>("sub1", &plugin1_1::new_request_1);
}

}}} //namespace gts::web::business
