#include <rttr/registration>
#include <rttr/library.h>
#include <asio.hpp>
#include <iostream>
#include <thread>
#include <future>

#ifdef GTS_ENABLE_SSL
# include <asio/ssl.hpp>
#endif //ssl

#ifdef _MSC_VER
# define DECL_EXPORT  __declspec(dllexport)
#elif defined(__GNUC__)
# define DECL_EXPORT  __attribute__((visibility("default")))
#else // other compiler
# define DECL_EXPORT
#endif //compiler

using namespace asio::ip;

namespace gts { namespace web { namespace business
{

class DECL_EXPORT plugin0
{
public:
	static std::shared_ptr<std::future<void>> init();
//	static std::shared_ptr<std::future<void>> exit();
	static void exit();
	static std::string view_status();

public:
	void set_version(const std::string &version);
	void set_method(const std::string &method);
	void set_path(const std::string &path);

public:
	void add_parameter(const std::string &key, const std::string &value);
	void add_header(const std::string &key, const std::string &value);

public:
	void call0(tcp::socket &socket, void *ssl);
	void call1(tcp::socket &socket, void *ssl);

private:
	std::string m_version;
	std::string m_method;
	std::string m_path;

private:
	std::unordered_map<std::string, std::string> m_parameters;
	std::unordered_map<std::string, std::string> m_headers;
};

std::shared_ptr<std::future<void>> plugin0::init()
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

//std::shared_ptr<std::future<void>> plugin0::exit()
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

void plugin0::exit()
{
	std::cerr << "plugin0: exit task ..." << std::endl;
}

std::string plugin0::view_status()
{
	return "web plugin: examples-plugin0: hello0\n";
}

void plugin0::set_version(const std::string &version)
{
	m_version = version;
}

void plugin0::set_method(const std::string &method)
{
	m_method = method;
}

void plugin0::set_path(const std::string &path)
{
	m_path = path;
}

void plugin0::add_parameter(const std::string &key, const std::string &value)
{
	m_parameters.emplace(key, value);
}

void plugin0::add_header(const std::string &key, const std::string &value)
{
	m_headers.emplace(key, value);
}

void plugin0::call0(tcp::socket &tcp_socket, void *ssl)
{
	std::string buf = "HTTP/1.1 200 OK\r\n"
					  "content-length: 11\r\n"
					  "content-type: text/plain; charset=utf-8\r\n"
					  "keep-alive: close\r\n"
					  "\r\n"
					  "hello world";

	asio::error_code error;
#ifdef GTS_ENABLE_SSL
	if( ssl )
	{
		typedef asio::ssl::stream<tcp::socket>  ssl_stream;
		ssl_stream ssl_socket(std::move(tcp_socket), reinterpret_cast<ssl_stream::native_handle_type>(ssl));

		ssl_socket.write_some(asio::buffer(buf), error);
		ssl_socket.next_layer().shutdown(tcp::socket::shutdown_both);
		ssl_socket.next_layer().close();
	}
	else
#else
	(void)(ssl);
#endif //ssl
	{
		tcp_socket.write_some(asio::buffer(buf), error);
		tcp_socket.shutdown(tcp::socket::shutdown_both);
		tcp_socket.close();
	}
}

void plugin0::call1(tcp::socket &tcp_socket, void *ssl)
{
	std::string buf = "HTTP/1.1 200 OK\r\n"
					  "content-length: 11\r\n"
					  "content-type: text/plain; charset=utf-8\r\n"
					  "keep-alive: close\r\n"
					  "\r\n"
					  "HELLO WORLD";

	asio::error_code error;
#ifdef GTS_ENABLE_SSL
	if( ssl )
	{
		typedef asio::ssl::stream<tcp::socket>  ssl_stream;
		ssl_stream ssl_socket(std::move(tcp_socket), reinterpret_cast<ssl_stream::native_handle_type>(ssl));

		ssl_socket.write_some(asio::buffer(buf), error);
		ssl_socket.next_layer().shutdown(tcp::socket::shutdown_both);
		ssl_socket.next_layer().close();
	}
	else
#else
	(void)(ssl);
#endif //ssl
	{
		tcp_socket.write_some(asio::buffer(buf), error);
		tcp_socket.shutdown(tcp::socket::shutdown_both);
		tcp_socket.close();
	}
}

}}} //namespace gts::web::business

using namespace gts::web::business;

RTTR_PLUGIN_REGISTRATION
{
	using namespace rttr;
	auto lib_name = library::get_library_name(reinterpret_cast<void*>(&plugin0::init)).to_string();

	registration::class_<plugin0>("gts.web.plugin./")
			.constructor<>()
			.method("set_version"  , &plugin0::set_version)
			.method("set_method"   , &plugin0::set_method)
			.method("set_path"     , &plugin0::set_path)
			.method("add_parameter", &plugin0::add_parameter)
			.method("add_header"   , &plugin0::add_header)
			.method("plugin0-0"    , &plugin0::call0)
			.method("plugin0-1"    , &plugin0::call1);

	registration::
			method("gts.web.plugin." + lib_name + ".init", &plugin0::init)
			.method("gts.web.plugin." + lib_name + ".exit", &plugin0::exit)
			.method("gts.web.plugin." + lib_name + ".view_status", &plugin0::view_status);
}
