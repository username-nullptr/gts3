#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include "service/service_io.h"
#include "gts/web_plugin_interface.h"
#include "gts/web_config_key.h"
#include "gts/algorithm.h"
#include "gts/settings.h"
#include "gts/log.h"
#include "app_info.h"

#include <rttr/library.h>
#include <cppfilesystem>
#include <fstream>

namespace gts { namespace web
{

template <class asio_socket>
class GTS_DECL_HIDDEN plugin_service
{
	GTS_DISABLE_COPY_MOVE(plugin_service)
	typedef service_io<asio_socket>  s_io;

public:
	plugin_service(s_io &sio);
	void call();

private:
	int _call(const std::string &class_name, const string_list &str_list);
	rttr::method get_call_method(rttr::type &type, string_list str_list);
	rttr::method get_call_method(rttr::type &type, const std::string &method_name);

private:
	s_io &m_sio;
	bool m_direct_pass_request = true;
	int m_call_method_id = 0;
};

class GTS_DECL_HIDDEN plugin_service_config
{
public:
	static std::list<rttr::library> library_list;
//	static std::list<rttr::type> type_list;

public:
	static void init();
	static void exit();
	static std::string view_status();
};

template <class asio_socket>
plugin_service<asio_socket>::plugin_service(s_io &sio) :
	m_sio(sio)
{
	m_sio.socket.non_blocking(false);
}

template <class asio_socket>
void plugin_service<asio_socket>::call()
{
	auto str_list = string_split(m_sio.url_name, "/");
	assert(not str_list.empty());

	if( _call("/", str_list) )
		return ;

	std::string class_name;
	while( not str_list.empty() )
	{
		class_name += "/" + str_list[0];
		str_list.pop_front();

		if( _call(class_name, str_list) )
			return ;
	}
	m_sio.return_to_null(http::hs_not_found);
}

template <class asio_socket>
int plugin_service<asio_socket>::_call(const std::string &class_name, const string_list &str_list)
{
	auto type = rttr::type::get_by_name(GTS_WEB_PLUGIN_INTERFACE + std::string(class_name.c_str() + 1, class_name.size() - 1));
	if( not type.is_valid() )
	{
		type = rttr::type::get_by_name(GTS_WEB_PLUGIN_INTERFACE + class_name);
		if( not type.is_valid() )
			return 0;
	}

	auto call_method = get_call_method(type, str_list);
	if( not call_method.is_valid() )
		return 0;

	auto obj = type.create();
	if( not obj.is_valid() )
	{
		log_error("Web plugin class: object create failed.");
		m_sio.return_to_null(http::hs_internal_server_error);
		return -1;
	}

	auto method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_ENV, {
									  rttr::type::get<std::string>(),
									  rttr::type::get<std::string>()
								  });
	if( method.is_valid() )
	{
		auto parameter = m_sio.request.parameters_string;
		if( parameter.empty() )
			parameter = "/";

		method.invoke(obj, std::string("REQUEST_METHOD")   , m_sio.request.method);
		method.invoke(obj, std::string("QUERY_STRING")     , parameter);
		method.invoke(obj, std::string("REMOTE_ADDR")      , m_sio.socket.remote_endpoint().address().to_string());
		method.invoke(obj, std::string("GATEWAY_INTERFACE"), std::string("RTTR/" RTTR_VERSION_STR));
		method.invoke(obj, std::string("SERVER_NAME")      , m_sio.socket.local_endpoint().address().to_string());
		method.invoke(obj, std::string("SERVER_PORT")      , fmt::format("{}", m_sio.socket.local_endpoint().port()));
		method.invoke(obj, std::string("SERVER_PROTOCOL")  , "HTTP/" + m_sio.request.version);
		method.invoke(obj, std::string("DOCUMENT_ROOT")    , service_io_config::resource_path);
		method.invoke(obj, std::string("SERVER_SOFTWARE")  , std::string("GTS/1.0(GTS/" GTS_VERSION_STR ")"));
	}

	if( m_direct_pass_request )
	{
		if( m_call_method_id == 0 ) {
			call_method.invoke(obj, std::move(m_sio.request), std::move(m_sio.socket));
		}
		else if( m_call_method_id == 1 )
		{
			call_method.invoke(obj, std::move(m_sio.request), std::move(m_sio.socket.next_layer()),
							   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
		}
		else if( m_call_method_id == 2 )
		{
			bool is_v6 = m_sio.socket.remote_endpoint().address().is_v6();
			call_method.invoke(obj, std::move(m_sio.request), m_sio.socket.release(),
							   reinterpret_cast<void*>(m_sio.socket.release_ssl()), is_v6);
		}
		else
		{
			call_method.invoke(obj, std::move(m_sio.request), m_sio.socket.release(),
							   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
		}
	}
	else
	{
		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_VERSION, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(obj, m_sio.request.version);

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_METHOD, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(obj, m_sio.request.method);

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_PATH, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(obj, m_sio.request.path);

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_PARAMETER, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			for(auto &header : m_sio.request.parameters)
				method.invoke(obj, header.first, header.second.to_string());
		}

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_HEADER, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			for(auto &para : m_sio.request.headers)
				method.invoke(obj, para.first, para.second);
		}

		if( not m_sio.request.body.empty() )
		{
			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_BODY, {rttr::type::get<std::string>()});
			if( method.is_valid() )
				method.invoke(obj, m_sio.request.body);
		}

		if( m_call_method_id == 0 ) {
			call_method.invoke(obj, std::move(m_sio.socket));
		}
		else if( m_call_method_id == 1 )
		{
			call_method.invoke(obj, std::move(m_sio.socket.next_layer()),
							   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
		}
		else if( m_call_method_id == 2 )
		{
			bool is_v6 = m_sio.socket.remote_endpoint().address().is_v6();
			call_method.invoke(obj, m_sio.socket.release(),
							   reinterpret_cast<void*>(m_sio.socket.release_ssl()), is_v6);
		}
		else
		{
			call_method.invoke(obj, m_sio.socket.release(),
							   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
		}
	}
	return true;
}

template <class asio_socket>
rttr::method plugin_service<asio_socket>::get_call_method(rttr::type &type, string_list str_list)
{
	if( str_list.empty() )
	{
		m_sio.request.path = "/";
		return get_call_method(type, "/");
	}

	std::string method_name;
	while( not str_list.empty() )
	{
		method_name += "/" + str_list[0];
		str_list.pop_front();

		m_sio.request.path = "/" + str_list.join("/");

		auto method = get_call_method(type, std::string(method_name.c_str() + 1, method_name.size() - 1));
		if( method.is_valid() )
			return method;

		method = get_call_method(type, method_name);
		if( method.is_valid() )
			return method;
	}
	return rttr::type::get_global_method("");
}

template <class asio_socket>
rttr::method plugin_service<asio_socket>::get_call_method(rttr::type &type, const std::string &method_name)
{
	auto method = type.get_method(method_name, {
									  rttr::type::get<http::request>(),
									  rttr::type::get<gts::socket<asio_socket>>()
								  });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 0;
		return method;
	}

#ifdef GTS_ENABLE_SSL
	method = type.get_method(method_name, {
								 rttr::type::get<http::request>(),
								 rttr::type::get<gts::socket<asio_socket>>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 0;
		return method;
	}
#endif //ssl

	method = type.get_method(method_name, {
								 rttr::type::get<http::request>(),
								 rttr::type::get<tcp::socket>(),
								 rttr::type::get<void*>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 1;
		return method;
	}

	method = type.get_method(method_name, {
								 rttr::type::get<http::request>(),
								 rttr::type::get<tcp::socket::native_handle_type>(),
								 rttr::type::get<void*>(),
								 rttr::type::get<bool>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 2;
		return method;
	}

	method = type.get_method(method_name, {
								 rttr::type::get<http::request>(),
								 rttr::type::get<tcp::socket::native_handle_type>(),
								 rttr::type::get<void*>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 3;
		return method;
	}

	method = type.get_method(method_name, {
								 rttr::type::get<gts::socket<asio_socket>>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 0;
		return method;
	}

#ifdef GTS_ENABLE_SSL
	method = type.get_method(method_name, {
								 rttr::type::get<gts::socket<asio_socket>>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 0;
		return method;
	}
#endif //ssl

	method = type.get_method(method_name, {
								 rttr::type::get<tcp::socket>(),
								 rttr::type::get<void*>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 1;
		return method;
	}

	method = type.get_method(method_name, {
								 rttr::type::get<tcp::socket::native_handle_type>(),
								 rttr::type::get<void*>(),
								 rttr::type::get<bool>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 2;
		return method;
	}

	method = type.get_method(method_name, {
								 rttr::type::get<tcp::socket::native_handle_type>(),
								 rttr::type::get<void*>()
							 });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 3;
		return method;
	}
	return method;
}

}} //namespace gts::web


#endif //PLUGIN_SERVICE_H
