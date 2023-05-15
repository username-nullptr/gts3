#include <rttr/registration>
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

class DECL_EXPORT plugin0
{
public:
	void set_version(const std::string &version);
	void set_method(const std::string &method);
	void set_path(const std::string &path);

public:
	void add_parameter(const std::string &key, const std::string &value);
	void add_header(const std::string &key, const std::string &value);

public:
	void call(tcp::socket::native_handle_type handle, int ipv);

private:
	std::string m_version;
	std::string m_method;
	std::string m_path;

private:
	std::unordered_map<std::string, std::string> m_parameters;
	std::unordered_map<std::string, std::string> m_headers;
};

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

inline void plugin0::call(tcp::socket::native_handle_type handle, int ipv)
{
	asio::io_context io;
	std::shared_ptr<tcp::socket> socket;

	if( ipv == 4 )
		socket = std::make_shared<tcp::socket>(io, tcp::v4(), handle);
	else
		socket = std::make_shared<tcp::socket>(io, tcp::v6(), handle);

	asio::error_code error;
	socket->write_some(asio::buffer(
						   "HTTP/1.1 200 OK\r\n"
						   "content-length: 11\r\n"
						   "keep-alive: close\r\n"
						   "\r\n"
						   "hello world"
						   ), error);

	socket->shutdown(tcp::socket::shutdown_both);
	socket->close();
}

void plugin0_dladdr_helper() {}

}}} //namespace gts::web::business

using namespace gts::web::business;

RTTR_PLUGIN_REGISTRATION
{
	Dl_info info;
	dladdr(reinterpret_cast<void*>(plugin0_dladdr_helper), &info);

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

	rttr::registration::class_<plugin0>("gts.web.plugin." + file_name)
			.constructor<>()
			.method("set_version"  , &plugin0::set_version)
			.method("set_method"   , &plugin0::set_method)
			.method("set_path"     , &plugin0::set_path)
			.method("add_parameter", &plugin0::add_parameter)
			.method("add_header"   , &plugin0::add_header)
			.method("call"         , &plugin0::call);
}
