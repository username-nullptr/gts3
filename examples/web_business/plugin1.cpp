#include <rttr/registration>
#include <fmt/format.h>
#include <asio.hpp>
#include <iostream>
#include <dlfcn.h>

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
	void call(tcp::socket::native_handle_type handle, int ipv);

private:
	std::string m_version;
	std::string m_method;
	std::string m_path;

private:
	std::unordered_map<std::string, std::string> m_parameters;
	std::unordered_map<std::string, std::string> m_headers;
	std::unordered_map<std::string, std::string> m_envs;
};

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

inline void plugin1::call(tcp::socket::native_handle_type handle, int ipv)
{
	std::cerr << std::endl;

	asio::io_context io;
	std::shared_ptr<tcp::socket> socket;

	if( ipv == 4 )
		socket = std::make_shared<tcp::socket>(io, tcp::v4(), handle);
	else
		socket = std::make_shared<tcp::socket>(io, tcp::v6(), handle);

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

	asio::error_code error;
	socket->write_some(asio::buffer(fmt::format(
						   "HTTP/1.1 200 OK\r\n"
						   "content-length: {}\r\n"
						   "keep-alive: close\r\n"
						   "\r\n", content.size()) +
						   content
						   ), error);

	socket->shutdown(tcp::socket::shutdown_both);
	socket->close();
}

void plugin1_dladdr_helper() {}

}}} //namespace gts::web::business

using namespace gts::web::business;

RTTR_PLUGIN_REGISTRATION
{
	Dl_info info;
	dladdr(reinterpret_cast<void*>(plugin1_dladdr_helper), &info);

	std::string file_name = info.dli_fname;
	auto pos = file_name.rfind("/");

	if( pos != std::string::npos )
		file_name.erase(0, pos + 1);

#ifdef __unix__

	if( file_name[0] == 'l' and file_name[1] == 'i' and file_name[2] == 'b' )
		file_name.erase(0,3);

	pos = file_name.find(".so");
	if( pos != std::string::npos )
		file_name.erase(pos);

#elif defined(_WINDOWS)

	pos = file_name.find(".dll");
	if( pos != std::string::npos )
		file_name.erase(pos);

#else //os
	// ... ...
#endif //os

	rttr::registration::class_<plugin1>("gts.web.plugin.plugin1")
			.constructor<>()
			.method("set_version"  , &plugin1::set_version)
			.method("set_method"   , &plugin1::set_method)
			.method("set_path"     , &plugin1::set_path)
			.method("add_parameter", &plugin1::add_parameter)
			.method("add_header"   , &plugin1::add_header)
			.method("add_env"      , &plugin1::add_env)
			.method("set_body"     , &plugin1::set_body)
			.method("call"         , &plugin1::call);
}
