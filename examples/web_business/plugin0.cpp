#include <gts/tcp_socket.h>
#include <gts/http/formatter.h>
#include <gts/web.h>
#include <iostream>

using namespace asio::ip;

namespace gts { namespace web { namespace business
{

using namespace http;

GTS_PLUGIN_ASYNC_INIT()
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

//GTS_PLUGIN_ASYNC_EXIT()
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

GTS_PLUGIN_EXIT(){
	std::cerr << "plugin0: exit task ..." << std::endl;
}

GTS_PLUGIN_VIEW_STATUS(){
	return "web plugin: examples-plugin0: hello0\n";
}

GTS_PLUGIN_HTTP_REQUEST_FILTER("plugin0", http::request &req)
{
	std::cerr << fmt::format("plugin0: request_filter: '{} ({:s})'.", req.path(), req.method()) << std::endl;
	return false;
}

GTS_PLUGIN_HTTP_REQUEST_HANDLE("plugin0", GET, http::response &res)
{
	gts_custom_log_error("test", "==================== 000000000000");
	res.write("hello world");
}

GTS_PLUGIN_HTTP_REQUEST_HANDLE("plugin0/sub0", GET, http::response &res)
{
	res.write("HELLO WORLD");
}

GTS_PLUGIN_HTTP_REQUEST_HANDLE("plugin0/sub1", GET, http::response &res)
{
	res.set_header(http::header::transfer_encoding, "chunked")
//			.set_chunk_attribute("aaa")
			.write("first -----\r\n")
//			.set_chunk_attributes({"bbb", 666})
			.write("second -----\r\n");
//			.chunk_end({{"aaa", "bbb"},{"ccc", 666}});
}

GTS_PLUGIN_HTTP_REQUEST_HANDLE("upload", PUT|POST, http::response &res, http::request &req)
{
	auto it = req.headers().find("upload-file-name");
	if( it == req.headers().end() )
	{
		res.set_status(http::hs_misdirected_request)
		   .write("421 upload-file-name is null");
	}
	else
	{
		std::error_code error;
#if 0
		while( req.can_read_body() )
		{
			auto buf = req.read_body(error);
			if( error )
			{
				res.set_status(http::hs_bad_request).write(error.message());
				return;
			}
			std::cerr << buf << std::endl << std::endl;
		}
		res.write("OK");
#else
		if( req.save_file(resource_root() + "upload/" + it->second, error) == false )
		{
			res.set_status(http::hs_bad_request).write(error.message());
			return ;
		}
		std::string body;
		for(auto &para : req.headers())
			body += fmt::format("(h) {} = {}\n", para.first, para.second);
		body += "\n";

		body.erase(body.size() - 2);
		res.write(body);
#endif
	}
}

}}} //namespace gts::web::business
