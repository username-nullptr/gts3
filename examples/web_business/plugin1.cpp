#include <rttr/registration>
#include <rttr/library.h>
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

class DECL_EXPORT plugin1
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
	void add_env(const std::string &key, const std::string &value);
	void set_body(const std::string &body);

public:
	template <typename asio_socket>
	void call(gts::socket<asio_socket> &sock);

private:
	std::string m_version;
	std::string m_method;
	std::string m_path;

private:
	std::unordered_map<std::string, std::string> m_parameters;
	std::unordered_map<std::string, std::string> m_headers;
	std::unordered_map<std::string, std::string> m_envs;
};

std::shared_ptr<std::future<void>> plugin1::init()
{
	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
	{
		for(int i=0; i<6; i++)
		{
			std::cerr << "plugin1: init task ..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(400));
		}
	}));
}

//std::shared_ptr<std::future<void>> plugin1::exit()
//{
//	return std::make_shared<std::future<void>>(std::async(std::launch::async, []
//	{
//		for(int i=0; i<5; i++)
//		{
//			std::cerr << "plugin1: exit task ..." << std::endl;
//			std::this_thread::sleep_for(std::chrono::milliseconds(300));
//		}
//	}));
//}

void plugin1::exit()
{
	std::cerr << "plugin1: exit task ..." << std::endl;
}

std::string plugin1::view_status()
{
	return "web plugin: examples-plugin1: hello1\n";
}

void plugin1::set_version(const std::string &version)
{
	m_version = version;
}

inline void plugin1::set_method(const std::string &method)
{
	m_method = method;
}

inline void plugin1::set_path(const std::string &path)
{
	std::cerr << "path: " << path << std::endl;
	m_path = path;
}

inline void plugin1::add_parameter(const std::string &key, const std::string &value)
{
	std::cerr << fmt::format("parameter: {} - {}", key, value) << std::endl;
	m_parameters.emplace(key, value);
}

inline void plugin1::add_header(const std::string &key, const std::string &value)
{
	std::cerr << fmt::format("header: {} - {}", key, value) << std::endl;
	m_headers.emplace(key, value);
}

inline void plugin1::add_env(const std::string &key, const std::string &value)
{
	std::cerr << fmt::format("env: {} - {}", key, value) << std::endl;
	m_envs.emplace(key, value);
}

inline void plugin1::set_body(const std::string&)
{

}

template <typename asio_socket>
inline void plugin1::call(gts::socket<asio_socket> &sock)
{
	std::cerr << std::endl;

	auto content = fmt::format("Ok!!!\npath = {}\n\n", m_path);

	for(auto &para : m_parameters)
		content += fmt::format("(p) {} = {}\n", para.first, para.second);
	content += "\n";

	for(auto &para : m_headers)
		content += fmt::format("(h) {} = {}\n", para.first, para.second);
	content += "\n";

	for(auto &para : m_envs)
		content += fmt::format("(e) {} = {}\n", para.first, para.second);
	content += "\n";

	content.erase(content.size() - 2);
	content = fmt::format("HTTP/1.1 200 OK\r\n"
						  "content-length: {}\r\n"
						  "content-type: text/plain; charset=utf-8\r\n"
						  "keep-alive: close\r\n"
						  "\r\n", content.size()) + content;

	asio::error_code error;
	sock.write_some(asio::buffer(content), error);
	sock.next_layer().shutdown(tcp::socket::shutdown_both);
	sock.next_layer().close();
}

}}} //namespace gts::web::business

using namespace gts::web::business;

RTTR_PLUGIN_REGISTRATION
{
	using namespace rttr;
	auto lib_name = library::get_library_name(reinterpret_cast<void*>(&plugin1::init)).to_string();

	registration::class_<plugin1>("gts.web.plugin." + lib_name)
			.constructor<>()
			.method("set_version"  , &plugin1::set_version)
			.method("set_method"   , &plugin1::set_method)
			.method("set_path"     , &plugin1::set_path)
			.method("add_parameter", &plugin1::add_parameter)
			.method("add_header"   , &plugin1::add_header)
			.method("add_env"      , &plugin1::add_env)
			.method("set_body"     , &plugin1::set_body)
			.method("call"         , &plugin1::call<tcp::socket>)
#ifdef GTS_ENABLE_SSL
			.method("call"         , &plugin1::call<gts::ssl_stream>)
#endif //ssl
			;

	registration::
			method("gts.web.plugin." + lib_name + ".init", &plugin1::init)
			.method("gts.web.plugin." + lib_name + ".exit", &plugin1::exit)
			.method("gts.web.plugin." + lib_name + ".view_status", &plugin1::view_status);
}
