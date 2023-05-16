#include "service.h"
#include "settings.h"
#include "app_info.h"
#include "gts/algorithm.h"
#include "gts/web_config_key.h"
#include "gts/web_plugin_interface.h"
#include "gts_log.h"

#include <rapidjson/document.h>
#include <rttr/library.h>
#include <cppfilesystem>
#include <fstream>

namespace gts { namespace web
{

static std::string g_resource_path = _GTS_WEB_DEFAULT_RC_PATH;

static std::list<rttr::library> g_library_list;

static std::list<rttr::type> g_type_list;

void service::call_plugins(const std::string &url_name)
{
	std::string plugin_name;
	std::string path;

	auto pos = url_name.find("/");
	if( pos == std::string::npos )
	{
		plugin_name = url_name;
		path = "/";
	}
	else
	{
		plugin_name = url_name.substr(0, pos);
		path = url_name.substr(pos);
	}

	for(auto &type : g_type_list)
	{
		if( type.get_name() != GTS_WEB_PLUGIN_INTERFACE + plugin_name )
			continue ;

		auto var = type.create();
		if( not var.is_valid() )
		{
			log_error("Web plugin class: object create failed.");
			return return_to_null(http::hs_internal_server_error);
		}

		int call_method_para_count = 0;
		auto call_method = type.get_method
						   (GTS_WEB_PLUGIN_INTERFACE_CALL, {
								rttr::type::get<tcp::socket::native_handle_type>(),
								rttr::type::get<int>()
							});
		if( call_method.is_valid() )
			call_method_para_count = 2;
		else
		{
			call_method = type.get_method
						  (GTS_WEB_PLUGIN_INTERFACE_CALL, {
							   rttr::type::get<tcp::socket::native_handle_type>()
						   });

			if( call_method.is_valid() )
				call_method_para_count = 1;
			else
			{
				log_error("Web plugin class::call is cannot be called.");
				return return_to_null(http::hs_internal_server_error);
			}
		}

		auto method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_VERSION, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(var, request.version());

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_METHOD, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(var, request.method());

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_PATH, {rttr::type::get<std::string>()});
		if( method.is_valid() )
			method.invoke(var, path);

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_PARAMETER, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			for(auto &header : request.parameters())
				method.invoke(var, header.first, header.second);
		}

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_HEADER, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			for(auto &para : request.headers())
				method.invoke(var, para.first, para.second);
		}

		method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_ADD_ENV, {
									 rttr::type::get<std::string>(),
									 rttr::type::get<std::string>()
								 });
		if( method.is_valid() )
		{
			auto parameter = request.parameters_string();
			if( parameter.empty() )
				parameter = "/";

			method.invoke(var, std::string("REQUEST_METHOD")   , request.method());
			method.invoke(var, std::string("QUERY_STRING")     , parameter);
			method.invoke(var, std::string("REMOTE_ADDR")      , socket.remote_endpoint().address().to_string());
			method.invoke(var, std::string("GATEWAY_INTERFACE"), std::string("RTTR/" RTTR_VERSION_STR));
			method.invoke(var, std::string("SERVER_NAME")      , socket.local_endpoint().address().to_string());
			method.invoke(var, std::string("SERVER_PORT")      , fmt::format("{}", socket.local_endpoint().port()));
			method.invoke(var, std::string("SERVER_PROTOCOL")  , "HTTP/" + request.version());
			method.invoke(var, std::string("DOCUMENT_ROOT")    , g_resource_path);
			method.invoke(var, std::string("SERVER_SOFTWARE")  , std::string("GTS/1.0(GTS/" GTS_VERSION_STR ")"));
		}

		if( not request.body().empty() )
		{
			method = type.get_method(GTS_WEB_PLUGIN_INTERFACE_SET_BODY, {rttr::type::get<std::string>()});
			if( method.is_valid() )
				method.invoke(var, request.body());
		}

		if( call_method_para_count == 1 )
			call_method.invoke(var, socket.native_handle());

		else if( socket.remote_endpoint().address().is_v4() )
			call_method.invoke(var, socket.native_handle(), 4);
		else
			call_method.invoke(var, socket.native_handle(), 6);
		return ;
	}
	return_to_null(http::hs_not_found);
}

void plugin_service_init()
{
	auto &_settings = settings::global_instance();
	g_resource_path = _settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_RC_PATH, g_resource_path);

	if( not starts_with(g_resource_path, "/") )
		g_resource_path = appinfo::dir_path() + g_resource_path;

	if( not ends_with(g_resource_path, _PS) )
		g_resource_path += _PS;

	auto json_file = _settings.read<std::string>(SINI_GROUP_WEB, SINI_WEB_PLUGINS_CONFIG, _GTS_WEB_DEFAULT_PLUGINS_CONFIG);
	if( not starts_with(json_file, "/") )
		json_file = appinfo::dir_path() + json_file;

	if( not fs::exists(json_file) )
	{
		log_debug("No have plugins service.");
		return ;
	}

	std::fstream file(json_file);
	if( not file.is_open() )
	{
		log_error("'{}' open failed.", json_file);
		return ;
	}

	auto file_size = fs::file_size(json_file);
	char *buf = new char[file_size + 1] {0};
	file_size = file.readsome(buf, file_size);

	rapidjson::Document json_object;
	bool has_error = json_object.Parse(buf).HasParseError();
	delete[] buf;

	if( has_error )
	{
		log_error("'{}': json parsing failed.", json_file);
		return ;
	}

	auto json_file_path = file_path(json_file);

	for(auto it=json_object.MemberBegin(); it!=json_object.MemberEnd(); ++it)
	{
		std::string type_name;
		std::string file_name;

		try {
			std::string file_path = it->value.GetString();
			if( not ends_with(file_path, _PS) )
				file_path += _PS;

			if( not starts_with(file_path, "/") )
				file_path = json_file_path + file_path;

			type_name = it->name.GetString();
			file_name = type_name;
#ifdef _WINDOWS
			file_name += ".dll";
#elif defined(__unix__)
			file_name = "lib" + file_name + ".so";
#else
			file_name += ".???";
#endif
			file_name = file_path + file_name;
		}
		catch(...) {
			log_warning("Web plugins config format(json) error.");
			continue;
		}

		g_library_list.emplace_back(file_name);
		auto &library = g_library_list.back();
		if( library.load() == false )
		{
			log_error("gts.web.plugin load failed: {}.", library.get_error_string());
			continue;
		}

		auto class_name = GTS_WEB_PLUGIN_INTERFACE + type_name;
		g_type_list.emplace_back(rttr::type::get_by_name(class_name));
		auto &type = g_type_list.back();

		if( not type.is_valid() )
		{
			log_error("gts.web.plugin error: service interface class is null.");
			g_type_list.pop_back();

			library.unload();
			g_library_list.pop_back();
			continue;
		}

		auto init_method_name = class_name + GTS_WEB_PLUGIN_INTERFACE_INIT;
		auto method = rttr::type::get_global_method(init_method_name);

		if( method.is_valid() )
			method.invoke({});
		else
		{
			method = rttr::type::get_global_method(init_method_name, {rttr::type::get<std::string>()});
			if( method.is_valid() )
				method.invoke({}, _settings.file_name());
		}
	}
}

void plugin_service_exit()
{
	if( g_library_list.empty() )
		return ;

	for(auto &type : g_type_list)
	{
		auto method = rttr::type::get_global_method(std::string(type.get_name()) + GTS_WEB_PLUGIN_INTERFACE_EXIT);
		if( method.is_valid() )
			method.invoke({});
	}
	g_type_list.clear();

	for(auto &library : g_library_list)
		library.unload();
	g_library_list.clear();
}

}} //namespace gts::web
