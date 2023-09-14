#include "server_tool.h"
#include "gts/registration.h"
#include "gts/application.h"
#include "gts/algorithm.h"
#include "app_info.h"

#include <nlohmann/json.hpp>
#include <rttr/library.h>
#include <cppformat>

using njson = nlohmann::json;

namespace gts
{

static site_info_map g_site_infos;

site_info_map get_site_infos()
{
	return g_site_infos;
}

site_info_map &server_get_site_infos()
{
	return g_site_infos;
}

#define _CHECK_RETURN_CALL(...) \
({ \
	if( method.get_return_type() == GTS_RTTR_TYPE(future_ptr) ) { \
		rttr::variant var = method.invoke(__VA_ARGS__); \
		futures.emplace_back(var.get_value<future_ptr>()); \
	} \
	else \
		method.invoke(__VA_ARGS__); \
})

template <typename Ins>
static void call_init(const rttr::method &method, Ins obj, std::list<future_ptr> &futures, const std::string &config_file)
{
	auto para_list = method.get_parameter_infos();
	if( para_list.empty() )
		_CHECK_RETURN_CALL(obj);

	else if( para_list.size() == 1 and para_list.begin()->get_type() == GTS_RTTR_TYPE(std::string) )
		_CHECK_RETURN_CALL(obj, config_file);
}

static bool load_library(const njson &name_json, const njson &lib_json, const std::string &json_file_path)
{
	try {
		auto file_name =
#ifdef _WINDOWS
				name_json.get<std::string>() + std::string(".dll");
#elif defined(__unix__)
				std::string("lib") + name_json.get<std::string>() + ".so";
#else
				name_json.get<std::string>() + ".???";
#endif
		std::string file_path = "./";
		auto it = lib_json.find("path");

		if( it != lib_json.end() )
		{
			file_path = it->get<std::string>();
			if( not str_ends_with(file_path, "/") )
				file_path += "/";

			if( not appinfo::is_absolute_path(file_path) )
				file_path = json_file_path + file_path;
		}
		rttr::library library(file_path + file_name);
		if( library.load() )
			return true;

		gts_log_error("gts.plugins load failed:") << library.get_error_string();
	}
	catch(...) {
		gts_log_fatal("gts::tcp_server::start: Plugins config format(json) error.");
	}
	return false;
}

void plugin_call_handle::init(const std::string &json_file, const std::string &config_file)
{
	std::ifstream file(json_file);
	auto plugins_json = njson::parse(file, nullptr, false);
	file.close();

	if( plugins_json.is_null() )
		gts_log_fatal("gts::tcp_server::start: Plugins json file read error.");

	else if( not plugins_json.is_object() )
		gts_log_fatal("gts::tcp_server::start: Plugins json format error. (not object)");

	auto envs_json = plugins_json["envs"];
	try {
		auto it = envs_json.begin();
		for(; it!=envs_json.end(); ++it)
		{
			auto &key = it.key();
			auto value = it.value().get<std::string>();

			gts_log_debug("setenv: '{}'-'{}'", key, value);
			app::setenv(key, value);
		}
	}
	catch(...) {}

	auto libs_array = plugins_json["libs"];
	if( not libs_array.is_array() )
		gts_log_fatal("gts::tcp_server::start: Plugins json format error. (not array)");

	auto json_file_path = file_path(json_file);
	std::size_t sum = 0;

	for(auto &lib_json : libs_array)
	{
		auto name_json_or_array = lib_json["name"];
		if( name_json_or_array.is_array() )
		{
			for(auto &name_obj : name_json_or_array)
			{
				if( load_library(name_obj, lib_json, json_file_path) )
					sum++;
			}
		}
		else if( load_library(name_json_or_array, lib_json, json_file_path) )
			sum++;
	}
	if( sum == 0 )
		gts_log_fatal("gts::tcp_server::start: No plugins found.");

	std::list<future_ptr> futures;
	{
		auto it = rttr::type::get_global_methods().begin();
		for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
		{
			auto &method = *it;
			if( str_starts_with(method.get_name().to_string(), "gts.plugin.init.") )
				call_init(method, rttr::instance(), futures, config_file);
		}
	}{
		auto it = rttr::type::get_types().begin();
		for(int i=0; it!=rttr::type::get_types().end(); it=std::next(rttr::type::get_types().begin(), ++i))
		{
			auto &type = *it;
			auto type_name = type.get_name().to_string();

			if( not str_starts_with(type_name, "gts.plugin.class.") or str_ends_with(type_name, "*") )
				continue;

			auto pair = registration::g_obj_hash.emplace(type, rttr::variant());
			if( pair.second == false )
				continue ;

			pair.first->second = type.create();
			auto &obj = pair.first->second;

			if( not obj.is_valid() )
			{
				gts_log_error("gts::tcp_server::start: Class create failed.");
				registration::g_obj_hash.erase(type);
				continue ;
			}
			auto method = type.get_method(fmt::format("init.{}", type.get_id()));
			if( method.is_valid() )
				call_init(method, obj, futures, config_file);
		}
	}
	for(auto &future : futures)
		future->wait();
}

template <typename Ins>
static void call_exit(const rttr::method &method, Ins obj, std::list<future_ptr> &futures)
{
	auto para_list = method.get_parameter_infos();
	if( para_list.empty() )
		_CHECK_RETURN_CALL(obj);
}

void plugin_call_handle::exit()
{
	std::list<future_ptr> futures;
	for(auto &pair : registration::g_obj_hash)
	{
		auto &type = pair.first;
		auto &obj = pair.second;

		auto method = type.get_method(fmt::format("exit.{}", type.get_id()));
		if( method.is_valid() )
			call_exit(method, obj, futures);
		obj.clear();
	}
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( str_starts_with(method.get_name().to_string(), "gts.web.plugin.exit.") )
			call_exit(method, rttr::instance(), futures);
	}
	for(auto &future : futures)
		future->wait();
	registration::g_obj_hash.clear();
}

static bool _new_connection_method_call(const std::string &method_suffix, tcp_socket *sock)
{
	auto method = rttr::type::get_global_method("gts.plugin." + method_suffix, {GTS_RTTR_TYPE(tcp_socket_ptr&)});
	if( method.is_valid() )
	{
		method.invoke({}, tcp_socket_ptr(sock));
		return true;
	}
	return false;
}

bool plugin_call_handle::new_connection(tcp_socket *sock, bool universal)
{
	if( universal or _new_connection_method_call(fmt::format("new_connection.{}", sock->local_endpoint().port()), sock) == false )
	{
		if( _new_connection_method_call("new_connection", sock) == false )
			return false;
	}
	return true;
}

std::string plugin_call_handle::view_status()
{
	std::string status;
	auto it = rttr::type::get_global_methods().begin();

	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( str_starts_with(method.get_name().to_string(), "gts.plugin.view_status.") and method.get_return_type() == GTS_RTTR_TYPE(std::string) )
			status += method.invoke({}).get_value<std::string>();
	}
	for(auto &pair : registration::g_obj_hash)
	{
		auto &type = pair.first;
		auto &obj = pair.second;

		auto method = type.get_method(fmt::format("view_status.{}", type.get_id()));
		if( method.is_valid() and method.get_return_type() == GTS_RTTR_TYPE(std::string) )
			status += method.invoke(obj).get_value<std::string>();
	}
	return status;
}

void plugin_call_handle::extension::init(const std::string &config_file)
{
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( not str_starts_with(method.get_name().to_string(), "gts.extension.plugin.init.") )
			continue;

		auto list = method.get_parameter_infos();
		if( list.size() == 0 )
			method.invoke({});

		else if( list.size() == 1 and list.begin()->get_type() == GTS_RTTR_TYPE(std::string) )
			method.invoke({}, config_file);
	}
}

void plugin_call_handle::extension::exit()
{
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( str_starts_with(method.get_name().to_string(), "gts.extension.plugin.exit.") and method.get_parameter_infos().empty() )
			method.invoke({});
	}
}

bool plugin_call_handle::extension::args_parsing(const string_list &args)
{
	std::size_t ct = 0;
	auto it = rttr::type::get_global_methods().begin();

	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( not str_starts_with(method.get_name().to_string(), "gts.extension.plugin.args_parsing.") )
			continue;

		auto para = method.get_parameter_infos();

		if( para.size() == 2 and para.begin()->get_type() == GTS_RTTR_TYPE(int) and (++para.begin())->get_type() == GTS_RTTR_TYPE(const char**) )
		{
			if( method.get_return_type() != GTS_RTTR_TYPE(bool) )
				continue;
			ct++;
			static auto other_args_vector = args.c_str_vector();
			if( method.invoke({}, static_cast<int>(other_args_vector.size()), other_args_vector.data()).to_bool() == false )
				return false;
		}
		else if( para.size() == 1 and para.begin()->get_type() == GTS_RTTR_TYPE(string_list) )
		{
			if( method.get_return_type() != GTS_RTTR_TYPE(bool) )
				continue ;
			ct++;
			if( method.invoke({}, args).to_bool() == false )
				return false;
		}
	}
	return ct > 0;
}

static std::string view_extension(const char *method_name)
{
	std::string result;
	auto it = rttr::type::get_global_methods().begin();

	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( not str_starts_with(method.get_name().to_string(), method_name) )
			continue;

		else if( method.get_return_type() != GTS_RTTR_TYPE(std::string) or not method.get_parameter_infos().empty() )
			continue;

		auto str = method.invoke({}).to_string();
		if( str.empty() )
			continue;

		int j = str.size() - 1;
		for(; j>=0; j--)
		{
			if( str[j] != '\n' )
				break;
		}
		result += str.substr(0, j + 1) + "\n";
	}
	if( not result.empty() )
		result.pop_back();
	return result;
}

std::string plugin_call_handle::extension::view_version()
{
	return view_extension("gts.extension.plugin.view_version.");
}

std::string plugin_call_handle::extension::view_help()
{
	return view_extension("gts.extension.plugin.view_help.");
}

} //namespace gts
