#include <rttr/registration>
#include <rttr/library.h>
#include <asio/ssl.hpp>
#include <asio.hpp>
#include <iostream>

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
	static void init();
	static void exit();

public:
	void set_version(const std::string &version);
	void set_method(const std::string &method);
	void set_path(const std::string &path);

public:
	void add_parameter(const std::string &key, const std::string &value);
	void add_header(const std::string &key, const std::string &value);

public:
	void call(tcp::socket::native_handle_type handle, void *ssl, int ipv);

private:
	std::string m_version;
	std::string m_method;
	std::string m_path;

private:
	std::unordered_map<std::string, std::string> m_parameters;
	std::unordered_map<std::string, std::string> m_headers;
};

void plugin0::init()
{

}

void plugin0::exit()
{

}

void plugin0::set_version(const std::string &version)
{
	m_version = version;
}

inline void plugin0::set_method(const std::string &method)
{
	m_method = method;
}

inline void plugin0::set_path(const std::string &path)
{
	m_path = path;
}

inline void plugin0::add_parameter(const std::string &key, const std::string &value)
{
	m_parameters.emplace(key, value);
}

inline void plugin0::add_header(const std::string &key, const std::string &value)
{
	m_headers.emplace(key, value);
}

inline void plugin0::call(tcp::socket::native_handle_type handle, void *ssl, int ipv)
{
	asio::io_context io;
	tcp::socket tcp_socket(io, ipv == 4? tcp::v4() : tcp::v6(), handle);

	std::string buf = "HTTP/1.1 200 OK\r\n"
					  "content-length: 11\r\n"
					  "content-type: text/plain; charset=utf-8\r\n"
					  "keep-alive: close\r\n"
					  "\r\n"
					  "hello world";

	asio::error_code error;
	if( ssl )
	{
		typedef asio::ssl::stream<tcp::socket>  ssl_stream;
		ssl_stream ssl_socket(std::move(tcp_socket), reinterpret_cast<ssl_stream::native_handle_type>(ssl));

		ssl_socket.write_some(asio::buffer(buf), error);
		ssl_socket.next_layer().shutdown(tcp::socket::shutdown_both);
		ssl_socket.next_layer().close();
	}
	else
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

	registration::class_<plugin0>("gts.web.plugin." + lib_name)
			.constructor<>()
			.method("set_version"  , &plugin0::set_version)
			.method("set_method"   , &plugin0::set_method)
			.method("set_path"     , &plugin0::set_path)
			.method("add_parameter", &plugin0::add_parameter)
			.method("add_header"   , &plugin0::add_header)
			.method("call"         , &plugin0::call);

	registration::
			method("gts.web.plugin." + lib_name + ".init", &plugin0::init)
			.method("gts.web.plugin." + lib_name + ".exit", &plugin0::exit);
}
