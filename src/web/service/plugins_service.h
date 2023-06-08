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
	DISABLE_COPY(plugin_service)
	typedef service_io<asio_socket>  s_io;

public:
	plugin_service(s_io &sio);
	void call();

private:
	rttr::method call_method_check(rttr::type &type);

private:
	s_io &m_sio;
	bool m_direct_pass_request = true;
	int m_call_method_id = 0;
};

class GTS_DECL_HIDDEN plugin_service_config
{
public:
	static std::list<rttr::library> library_list;
	static std::list<rttr::type> type_list;

public:
	static void init(const basic_string_list &args);
	static void exit();
	static std::string view_status();
};

template <class asio_socket>
plugin_service<asio_socket>::plugin_service(s_io &sio) :
	m_sio(sio)
{

}

template <class asio_socket>
void plugin_service<asio_socket>::call()
{
	std::string plugin_name;
	auto pos = m_sio.url_name.find("/");

	if( pos == std::string::npos )
	{
		plugin_name = m_sio.url_name;
		m_sio.request.path = "/";
	}
	else
	{
		plugin_name = m_sio.url_name.substr(0, pos);
		m_sio.request.path = m_sio.url_name.substr(pos);
	}

	for(auto &type : plugin_service_config::type_list)
	{
		if( type.get_name() != GTS_WEB_PLUGIN_INTERFACE + plugin_name )
			continue ;

		auto var = type.create();
		if( not var.is_valid() )
		{
			log_error("Web plugin class: object create failed.");
			return m_sio.return_to_null(http::hs_internal_server_error);
		}

		auto call_method = call_method_check(type);
		if( not call_method.is_valid() )
			return m_sio.return_to_null(http::hs_internal_server_error);

		auto method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_ENV, {
										  rttr::type::get<std::string>(),
										  rttr::type::get<std::string>()
									  });
		if( method.is_valid() )
		{
			auto parameter = m_sio.request.parameters_string;
			if( parameter.empty() )
				parameter = "/";

			method.invoke(var, std::string("REQUEST_METHOD")   , m_sio.request.method);
			method.invoke(var, std::string("QUERY_STRING")     , parameter);
			method.invoke(var, std::string("REMOTE_ADDR")      , m_sio.socket.remote_endpoint().address().to_string());
			method.invoke(var, std::string("GATEWAY_INTERFACE"), std::string("RTTR/" RTTR_VERSION_STR));
			method.invoke(var, std::string("SERVER_NAME")      , m_sio.socket.local_endpoint().address().to_string());
			method.invoke(var, std::string("SERVER_PORT")      , fmt::format("{}", m_sio.socket.local_endpoint().port()));
			method.invoke(var, std::string("SERVER_PROTOCOL")  , "HTTP/" + m_sio.request.version);
			method.invoke(var, std::string("DOCUMENT_ROOT")    , service_io_config::resource_path);
			method.invoke(var, std::string("SERVER_SOFTWARE")  , std::string("GTS/1.0(GTS/" GTS_VERSION_STR ")"));
		}

		if( m_direct_pass_request )
		{
			if( m_call_method_id == 0 )
			{
				call_method.invoke(var, m_sio.request, std::move(m_sio.socket.next_layer()),
								   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
			}
			else if( m_call_method_id == 1 )
			{
				bool is_v6 = m_sio.socket.remote_endpoint().address().is_v6();
				call_method.invoke(var, m_sio.request, m_sio.socket.release(),
								   reinterpret_cast<void*>(m_sio.socket.release_ssl()), is_v6);
			}
			else
			{
				call_method.invoke(var, m_sio.request, m_sio.socket.release(),
								   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
			}
		}
		else
		{
			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_VERSION, {rttr::type::get<std::string>()});
			if( method.is_valid() )
				method.invoke(var, m_sio.request.version);

			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_METHOD, {rttr::type::get<std::string>()});
			if( method.is_valid() )
				method.invoke(var, m_sio.request.method);

			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_PATH, {rttr::type::get<std::string>()});
			if( method.is_valid() )
				method.invoke(var, m_sio.request.path);

			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_PARAMETER, {
										 rttr::type::get<std::string>(),
										 rttr::type::get<std::string>()
									 });
			if( method.is_valid() )
			{
				for(auto &header : m_sio.request.parameters)
					method.invoke(var, header.first, header.second);
			}

			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_HEADER, {
										 rttr::type::get<std::string>(),
										 rttr::type::get<std::string>()
									 });
			if( method.is_valid() )
			{
				for(auto &para : m_sio.request.headers)
					method.invoke(var, para.first, para.second);
			}

			if( not m_sio.request.body.empty() )
			{
				method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_BODY, {rttr::type::get<std::string>()});
				if( method.is_valid() )
					method.invoke(var, m_sio.request.body);
			}

			if( m_call_method_id == 0 )
			{
				call_method.invoke(var, std::move(m_sio.socket.next_layer()),
								   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
			}
			else if( m_call_method_id == 1 )
			{
				bool is_v6 = m_sio.socket.remote_endpoint().address().is_v6();
				call_method.invoke(var, m_sio.socket.release(),
								   reinterpret_cast<void*>(m_sio.socket.release_ssl()), is_v6);
			}
			else
			{
				call_method.invoke(var, m_sio.socket.release(),
								   reinterpret_cast<void*>(m_sio.socket.release_ssl()));
			}
		}
		return ;
	}
	m_sio.return_to_null(http::hs_not_found);
}

template<class asio_socket>
rttr::method plugin_service<asio_socket>::call_method_check(rttr::type &type)
{
	auto method = type.get_method
				  (GTS_WEB_PLUGIN_INTERFACE_CALL, {
					   rttr::type::get<http::request>(),
					   rttr::type::get<tcp::socket>(),
					   rttr::type::get<void*>()
				   });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 0;
		return method;
	}

	method = type.get_method
			 (GTS_WEB_PLUGIN_INTERFACE_CALL, {
				  rttr::type::get<http::request>(),
				  rttr::type::get<tcp::socket::native_handle_type>(),
				  rttr::type::get<void*>(),
				  rttr::type::get<bool>()
			  });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 1;
		return method;
	}

	method = type.get_method
				  (GTS_WEB_PLUGIN_INTERFACE_CALL, {
					   rttr::type::get<http::request>(),
					   rttr::type::get<tcp::socket::native_handle_type>(),
					   rttr::type::get<void*>()
				   });
	if( method.is_valid() )
	{
		m_direct_pass_request = true;
		m_call_method_id = 2;
		return method;
	}

	method = type.get_method
			 (GTS_WEB_PLUGIN_INTERFACE_CALL, {
				  rttr::type::get<tcp::socket>(),
				  rttr::type::get<void*>()
			  });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 0;
		return method;
	}

	method = type.get_method
			 (GTS_WEB_PLUGIN_INTERFACE_CALL, {
				  rttr::type::get<tcp::socket::native_handle_type>(),
				  rttr::type::get<void*>(),
				  rttr::type::get<bool>()
			  });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 1;
		return method;
	}

	method = type.get_method
			 (GTS_WEB_PLUGIN_INTERFACE_CALL, {
				  rttr::type::get<tcp::socket::native_handle_type>(),
				  rttr::type::get<void*>()
			  });
	if( method.is_valid() )
	{
		m_direct_pass_request = false;
		m_call_method_id = 2;
		return method;
	}

	log_error("Web plugin class::call is cannot be called.");
	return method;
}

}} //namespace gts::web


#endif //PLUGIN_SERVICE_H
