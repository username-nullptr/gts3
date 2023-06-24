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
	void new_request(tcp_socket_ptr socket, http::request &&request, environments &&envs);
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

void plugin1_0::new_request(tcp_socket_ptr socket, http::request &&request, environments &&envs)
{
	std::cerr << std::endl;

	auto content = fmt::format("Ok!!!\npath = {}\n\n", request.path);

	for(auto &para : request.parameters)
		content += fmt::format("(p) {} = {}\n", para.first, para.second.to_string());
	content += "\n";

	for(auto &para : request.headers)
		content += fmt::format("(h) {} = {}\n", para.first, para.second);
	content += "\n";

	for(auto &para : envs)
		content += fmt::format("(e) {} = {}\n", para.first, para.second);
	content += "\n";

	content.erase(content.size() - 2);
	content = fmt::format("HTTP/1.1 200 OK\r\n"
						  "content-length: {}\r\n"
						  "content-type: text/plain; charset=utf-8\r\n"
						  "connection: close\r\n"
						  "\r\n", content.size()) + content;

	asio::error_code error;
	socket->write_some(content, error);
	socket->close();
}

class GTS_DECL_EXPORT plugin1_1
{
public:
	std::shared_ptr<std::future<void>> init();
	void exit();
	void new_request_0(tcp_socket_ptr socket);
	void new_request_1(tcp_socket_ptr socket);
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

void plugin1_1::new_request_0(tcp_socket_ptr socket)
{
	static const std::string str = "HTTP/1.1 200 OK\r\n"
								   "content-length: 12\r\n"
								   "content-type: text/plain; charset=utf-8\r\n"
								   "connection: close\r\n"
								   "\r\n"
								   "plugin1-1::0";
	asio::error_code error;
	socket->write_some(str, error);
	socket->close();
}

void plugin1_1::new_request_1(tcp_socket_ptr socket)
{
	static const std::string str = "HTTP/1.1 200 OK\r\n"
								   "content-length: 12\r\n"
								   "content-type: text/plain; charset=utf-8\r\n"
								   "connection: close\r\n"
								   "\r\n"
								   "PLUGIN1-1::1";
	asio::error_code error;
	socket->write_some(str, error);
	socket->close();
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
