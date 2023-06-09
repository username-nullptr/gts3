#include <rttr/registration>
#include <rttr/library.h>
#include <fmt/format.h>
#include <asio.hpp>
#include <iostream>
#include <dlfcn.h>

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
	static void init();
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
	void call(tcp::socket::native_handle_type handle, void *ssl, bool ipv6);

private:
	std::string m_version;
	std::string m_method;
	std::string m_path;

private:
	std::unordered_map<std::string, std::string> m_parameters;
	std::unordered_map<std::string, std::string> m_headers;
	std::unordered_map<std::string, std::string> m_envs;
};

void plugin1::init()
{

}

void plugin1::exit()
{

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

inline void plugin1::call(tcp::socket::native_handle_type handle, void *ssl, bool ipv6)
{
	std::cerr << std::endl;

	asio::io_context io;
	tcp::socket tcp_socket(io, ipv6? tcp::v6() : tcp::v4(), handle);

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
#ifdef GTS_ENABLE_SSL
	if( ssl )
	{
		typedef asio::ssl::stream<tcp::socket>  ssl_stream;
		ssl_stream ssl_socket(std::move(tcp_socket), reinterpret_cast<ssl_stream::native_handle_type>(ssl));

		ssl_socket.write_some(asio::buffer(content), error);
		ssl_socket.next_layer().shutdown(tcp::socket::shutdown_both);
		ssl_socket.next_layer().close();
	}
	else
#else
	(void)(ssl);
#endif //ssl
	{
		tcp_socket.write_some(asio::buffer(content), error);
		tcp_socket.shutdown(tcp::socket::shutdown_both);
		tcp_socket.close();
	}
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
			.method("call"         , &plugin1::call);

	registration::
			method("gts.web.plugin." + lib_name + ".init", &plugin1::init)
			.method("gts.web.plugin." + lib_name + ".exit", &plugin1::exit)
			.method("gts.web.plugin." + lib_name + ".view_status", &plugin1::view_status);
}
