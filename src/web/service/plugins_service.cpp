#include "plugins_service.h"
#include "json.h"
#include <iostream>

namespace gts { namespace web
{

std::list<rttr::library> plugin_service_config::library_list;

std::list<rttr::type> plugin_service_config::type_list;

static void call_init(const std::string &class_name)
{
	auto init_method_name = class_name + GTS_WEB_PLUGIN_INTERFACE_INIT;

	auto method = rttr::type::get_global_method(init_method_name);
	if( method.is_valid() and method.get_parameter_infos().size() == 0 )
	{
		method.invoke({});
		return ;
	}

	method = rttr::type::get_global_method(init_method_name, {rttr::type::get<std::string>()});
	if( method.is_valid() )
		method.invoke({}, settings::global_instance().file_name());
}

void plugin_service_config::init()
{
	auto &_settings = settings::global_instance();
	auto json_file = _settings.read<std::string>
					 (SINI_GROUP_WEB, SINI_WEB_PLUGINS_CONFIG, _GTS_WEB_DEFAULT_PLUGINS_CONFIG);

	if( json_file.empty() )
		return ;
	json_file = appinfo::absolute_path(json_file);

	rapidjson::Document json_object;
	auto errstr = rapidjson::from_file(json_file, json_object);

	if( not errstr.empty() )
	{
		log_error("Json file read failed: {}.", errstr);
		return ;
	}
	auto json_file_path = file_path(json_file);

	for(auto it=json_object.MemberBegin(); it!=json_object.MemberEnd(); ++it)
	{
		std::string type_name;
		std::string file_name;

		try {
			std::string file_path = it->value.GetString();
			if( not ends_with(file_path, "/") )
				file_path += "/";

			if( not appinfo::is_absolute_path(file_path) )
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

		library_list.emplace_back(file_name);
		auto &library = library_list.back();
		if( library.load() == false )
		{
			log_error("gts.web.plugin load failed: {}.", library.get_error_string());
			continue;
		}

		auto class_name = GTS_WEB_PLUGIN_INTERFACE + type_name;
		type_list.emplace_back(rttr::type::get_by_name(class_name));
		auto &type = type_list.back();

		if( not type.is_valid() )
		{
			log_error("gts.web.plugin error: service interface class is null.");
			type_list.pop_back();

			library.unload();
			library_list.pop_back();
			continue;
		}
		call_init(class_name);
	}
}

void plugin_service_config::exit()
{
	if( library_list.empty() )
		return ;

	for(auto &type : type_list)
	{
		auto method = rttr::type::get_global_method(std::string(type.get_name()) + GTS_WEB_PLUGIN_INTERFACE_EXIT);
		if( method.is_valid() )
			method.invoke({});
	}
	type_list.clear();

	for(auto &library : library_list)
		library.unload();
	library_list.clear();
}

std::string plugin_service_config::view_status()
{
	std::string result;
	for(auto &type : plugin_service_config::type_list)
	{
		auto method = rttr::type::get_global_method(std::string(type.get_name()) + GTS_WEB_PLUGIN_INTERFACE_VIEW_STATUS);
		if( method.is_valid() and method.get_return_type() == rttr::type::get<std::string>() )
			result += method.invoke({}).get_value<std::string>();
	}
	if( not result.empty() and result[0] != '\n' )
		result = "\n" + result;
	return result;
}

}} //namespace gts::web
