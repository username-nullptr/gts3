#include "plugins_service.h"

namespace gts { namespace web
{

std::list<rttr::library> plugin_service_config::library_list;

std::list<rttr::type> plugin_service_config::type_list;

void plugin_service_config::init()
{
	auto &_settings = settings::global_instance();
	auto json_file = _settings.read<std::string>
					 (SINI_GROUP_WEB, SINI_WEB_PLUGINS_CONFIG, _GTS_WEB_DEFAULT_PLUGINS_CONFIG);

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

}} //namespace gts::web
