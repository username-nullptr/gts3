#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include "service/service_io.h"
#include "gts/web_plugin_interface.h"
#include "gts/web_config_key.h"
#include "gts/algorithm.h"
#include "settings.h"
#include "app_info.h"
#include "gts_log.h"

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
	s_io &m_sio;
};

class GTS_DECL_HIDDEN plugin_service_config
{
public:
	static std::list<rttr::library> library_list;
	static std::list<rttr::type> type_list;

public:
	static void init();
	static void exit();
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
	std::string path;

	auto pos = m_sio.url_name.find("/");
	if( pos == std::string::npos )
	{
		plugin_name = m_sio.url_name;
		path = "/";
	}
	else
	{
		plugin_name = m_sio.url_name.substr(0, pos);
		path = m_sio.url_name.substr(pos);
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

		int call_method_para_count = 0;
		auto call_method = type.get_method
						   (GTS_WEB_PLUGIN_INTERFACE_CALL, {
								rttr::type::get<tcp::socket::native_handle_type>(),
								rttr::type::get<void*>(),
								rttr::type::get<int>()
							});
		if( call_method.is_valid() )
			call_method_para_count = 3;
		else
		{
			call_method = type.get_method
						  (GTS_WEB_PLUGIN_INTERFACE_CALL, {
							   rttr::type::get<tcp::socket::native_handle_type>(),
							   rttr::type::get<void*>()
						   });

			if( call_method.is_valid() )
				call_method_para_count = 2;
			else
			{
				log_error("Web plugin class::call is cannot be called.");
				return m_sio.return_to_null(http::hs_internal_server_error);
			}
		}

		auto method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_VERSION, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(var, m_sio.request.version());

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_METHOD, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(var, m_sio.request.method());

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_PATH, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(var, path);

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_PARAMETER, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			for(auto &header : m_sio.request.parameters())
				method.invoke(var, header.first, header.second);
		}

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_HEADER, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			for(auto &para : m_sio.request.headers())
				method.invoke(var, para.first, para.second);
		}

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_ENV, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			auto parameter = m_sio.request.parameters_string();
			if( parameter.empty() )
				parameter = "/";

			method.invoke(var, std::string("REQUEST_METHOD")   , m_sio.request.method());
			method.invoke(var, std::string("QUERY_STRING")     , parameter);
			method.invoke(var, std::string("REMOTE_ADDR")      , m_sio.socket.remote_endpoint().address().to_string());
			method.invoke(var, std::string("GATEWAY_INTERFACE"), std::string("RTTR/" RTTR_VERSION_STR));
			method.invoke(var, std::string("SERVER_NAME")      , m_sio.socket.local_endpoint().address().to_string());
			method.invoke(var, std::string("SERVER_PORT")      , fmt::format("{}", m_sio.socket.local_endpoint().port()));
			method.invoke(var, std::string("SERVER_PROTOCOL")  , "HTTP/" + m_sio.request.version());
			method.invoke(var, std::string("DOCUMENT_ROOT")    , service_io_config::resource_path);
			method.invoke(var, std::string("SERVER_SOFTWARE")  , std::string("GTS/1.0(GTS/" GTS_VERSION_STR ")"));
		}

		if( not m_sio.request.body().empty() )
		{
			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_BODY, {rttr::type::get<std::string>()});
			if( method.is_valid() )
				method.invoke(var, m_sio.request.body());
		}

		if( call_method_para_count == 2 )
			call_method.invoke(var, m_sio.socket.release(), reinterpret_cast<void*>(m_sio.socket.release_ssl()));

		else if( m_sio.socket.remote_endpoint().address().is_v4() )
			call_method.invoke(var, m_sio.socket.release(), reinterpret_cast<void*>(m_sio.socket.release_ssl()), 4);
		else
			call_method.invoke(var, m_sio.socket.release(), reinterpret_cast<void*>(m_sio.socket.release_ssl()), 6);
		return ;
	}
	m_sio.return_to_null(http::hs_not_found);
}
}} //namespace gts::web


#endif //PLUGIN_SERVICE_H
