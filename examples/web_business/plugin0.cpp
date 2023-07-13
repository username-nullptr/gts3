#include <gts/tcp_socket.h>
#include <gts/http/fmt_formatter.h>
#include <gts/web.h>
#include <iostream>

using namespace asio::ip;

namespace gts { namespace web { namespace business
{

GTS_DECL_EXPORT future_ptr init()
{
	return make_future_ptr(std::async(std::launch::async,[]
	{
		for(int i=0; i<3; i++)
		{
			std::cerr << "plugin0: init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}));
}

//GTS_DECL_EXPORT staic std::shared_ptr<std::future<void>> exit()
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

GTS_DECL_EXPORT void exit()
{
	std::cerr << "plugin0: exit task ..." << std::endl;
}

GTS_DECL_EXPORT std::string view_status()
{
	return "web plugin: examples-plugin0: hello0\n";
}

GTS_DECL_EXPORT bool request_filter(http::request &request)
{
	std::cerr << fmt::format("plugin0: request_filter: '{} ({:s})'.", request.path(), request.method()) << std::endl;
	return false;
}

GTS_DECL_EXPORT void new_request_0(http::response &response)
{
	response.write("hello world");
}

GTS_DECL_EXPORT void new_request_1(http::response &response)
{
	response.write("HELLO WORLD");
}

GTS_DECL_EXPORT void save_file(http::response &response, http::request &request)
{
	auto it = request.headers().find("upload-file-name");
	if( it == request.headers().end() )
	{
		response.set_status(http::hs_misdirected_request)
				.write("421 upload-file-name is null");
	}
	else
	{
		std::error_code error;
		if( request.save_file(resource_root() + "upload/" + it->second, error) == false )
		{
			response.set_status(http::hs_bad_request).write(error.message());
			return ;
		}
		std::string body;
		for(auto &para : request.headers())
			body += fmt::format("(h) {} = {}\n", para.first, para.second);
		body += "\n";

		body.erase(body.size() - 2);
		response.write(body);
	}
}

}}} //namespace gts::web::business

GTS_PLUGIN_REGISTRATION
{
	using namespace gts::http;
	using namespace gts::web;

	registration()
			.init_method(business::init)
			.exit_method(business::exit)
			.view_status_method(business::view_status)
			.filter_method("/", business::request_filter)
			.request_handle_method<GET>("plugin0", business::new_request_0)
			.request_handle_method<GET>("plugin0/sub", business::new_request_1)
			.request_handle_method<PUT,POST>("upload", business::save_file);
}
