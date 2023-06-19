#include <rttr/registration>
#include <rttr/library.h>

#include <gts/http/request.h>
#include <gts/socket.h>

#include <fmt/format.h>
#include <iostream>
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

class DECL_EXPORT plugin2
{
public:
	static std::shared_ptr<std::future<void>> init();
//	static std::shared_ptr<std::future<void>> exit();
	static void exit();
	static std::string view_status();

public:
	void add_env(const std::string &key, const std::string &value);

public:
	template <typename asio_socket>
	void call0(http::request &&request, gts::socket<asio_socket> &sock);

	template <typename asio_socket>
	void call1(http::request &&request, gts::socket<asio_socket> &sock);

private:
	std::unordered_map<std::string, std::string> m_envs;
};

std::shared_ptr<std::future<void>> plugin2::init()
{
	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
	{
		for(int i=0; i<2; i++)
		{
			std::cerr << "plugin2: init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}));
}

//std::shared_ptr<std::future<void>> plugin2::exit()
//{
//	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
//	{
//		for(int i=0; i<5; i++)
//		{
//			std::cerr << "plugin2: exit task ..." << std::endl;
//			std::this_thread::sleep_for(std::chrono::milliseconds(300));
//		}
//	}));
//}

void plugin2::exit()
{
	std::cerr << "plugin2: exit task ..." << std::endl;
}

std::string plugin2::view_status()
{
	return "web plugin: examples-plugin2: hello2\n";
}

void plugin2::add_env(const std::string &key, const std::string &value)
{
	std::cerr << fmt::format("env: {} - {}", key, value) << std::endl;
	m_envs.emplace(key, value);
}

template <typename asio_socket>
void plugin2::call0(http::request&&, gts::socket<asio_socket> &sock)
{
	asio::error_code error;
	sock.write_some(asio::buffer("HTTP/1.1 200 OK\r\n"
								 "content-length: 9\r\n"
								 "content-type: text/plain; charset=utf-8\r\n"
								 "keep-alive: close\r\n"
								 "\r\n"
								 "plugin2-0"
								 ), error);
	sock.shutdown(tcp::socket::shutdown_both);
	sock.close();
}

template <typename asio_socket>
void plugin2::call1(http::request&&, gts::socket<asio_socket> &sock)
{
	asio::error_code error;
	sock.write_some(asio::buffer("HTTP/1.1 200 OK\r\n"
								 "content-length: 9\r\n"
								 "content-type: text/plain; charset=utf-8\r\n"
								 "keep-alive: close\r\n"
								 "\r\n"
								 "plugin2-1"
								 ), error);
	sock.shutdown(tcp::socket::shutdown_both);
	sock.close();
}

class DECL_EXPORT plugin2_test2
{
public:
	void add_env(const std::string &key, const std::string &value);

public:
	template <typename asio_socket>
	void call0(http::request &&request, gts::socket<asio_socket> &sock);

	template <typename asio_socket>
	void call1(http::request &&request, gts::socket<asio_socket> &sock);

private:
	std::unordered_map<std::string, std::string> m_envs;
};

void plugin2_test2::add_env(const std::string &key, const std::string &value)
{
	std::cerr << fmt::format("env: {} - {}", key, value) << std::endl;
	m_envs.emplace(key, value);
}

template <typename asio_socket>
void plugin2_test2::call0(http::request&&, gts::socket<asio_socket> &sock)
{
	asio::error_code error;
	sock.write_some(asio::buffer("HTTP/1.1 200 OK\r\n"
								 "content-length: 9\r\n"
								 "content-type: text/plain; charset=utf-8\r\n"
								 "keep-alive: close\r\n"
								 "\r\n"
								 "plugin2-2"
								 ), error);
	sock.shutdown(tcp::socket::shutdown_both);
	sock.close();
}

template <typename asio_socket>
void plugin2_test2::call1(http::request&&, gts::socket<asio_socket> &sock)
{
	asio::error_code error;
	sock.write_some(asio::buffer("HTTP/1.1 200 OK\r\n"
								 "content-length: 9\r\n"
								 "content-type: text/plain; charset=utf-8\r\n"
								 "keep-alive: close\r\n"
								 "\r\n"
								 "plugin2-3"
								 ), error);
	sock.shutdown(tcp::socket::shutdown_both);
	sock.close();
}

}}} //namespace gts::web::business

using namespace gts::web::business;

RTTR_PLUGIN_REGISTRATION
{
	using namespace rttr;
	auto lib_name = library::get_library_name(reinterpret_cast<void*>(&plugin2::init)).to_string();

	registration::class_<plugin2>("gts.web.plugin." + lib_name)
			.constructor<>()
			.method("add_env", &plugin2::add_env)

			.method("test0", &plugin2::call0<tcp::socket>)
			.method("test0", &plugin2::call0<gts::ssl_stream>)
#ifdef GTS_ENABLE_SSL
			.method("test/test1", &plugin2::call1<tcp::socket>)
			.method("test/test1", &plugin2::call1<gts::ssl_stream>)
#endif //ssl
			;

	registration::class_<plugin2_test2>("gts.web.plugin." + lib_name + "/test")
			.constructor<>()
			.method("add_env", &plugin2_test2::add_env)

			.method("test2", &plugin2_test2::call0<tcp::socket>)
			.method("test2", &plugin2_test2::call0<gts::ssl_stream>)
#ifdef GTS_ENABLE_SSL
			.method("test/test3", &plugin2_test2::call1<tcp::socket>)
			.method("test/test3", &plugin2_test2::call1<gts::ssl_stream>)
#endif //ssl
			;

	registration::
			method("gts.web.plugin." + lib_name + ".init", &plugin2::init)
			.method("gts.web.plugin." + lib_name + ".exit", &plugin2::exit)
			.method("gts.web.plugin." + lib_name + ".view_status", &plugin2::view_status);
}
