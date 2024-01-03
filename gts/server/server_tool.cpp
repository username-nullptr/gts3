
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include "server_tool.h"
#include "gts/private/app_info.h"
#include "gts/coro_for_asio.h"
#include "gts/registration.h"
#include "gts/execution.h"
#include "gts/algorithm.h"

#include <nlohmann/json.hpp>
#include <cppformat>

using njson = nlohmann::json;

GTS_NAMESPACE_BEGIN

using coro_ptr = coroutine_ptr<void()>;

static site_info_map g_site_infos;

site_info_map get_site_infos()
{
	return g_site_infos;
}

site_info_map &server_get_site_infos()
{
	return g_site_infos;
}

template <typename Ins>
static coro_ptr call_init(const rttr::method &method, Ins obj, const std::string &config_file)
{
	return start_coroutine([&]
	{
		auto para_list = method.get_parameter_infos();
		if( para_list.empty() )
			method.invoke(obj);

		else if( para_list.size() == 1 and para_list.begin()->get_type() == GTS_RTTR_TYPE(std::string) )
			method.invoke(obj, config_file);
	});
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

			if( not app::is_absolute_path(file_path) )
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

	std::list<coro_ptr> coros;
	{
		auto it = rttr::type::get_global_methods().begin();
		for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
		{
			auto &method = *it;
			if( str_starts_with(method.get_name().to_string(), "gts.plugin.init.") )
				coros.emplace_back(call_init(method, rttr::instance(), config_file));
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
			if( not pair.second )
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
				coros.emplace_back(call_init(method, obj, config_file));
		}
	}
	auto &exe = execution::instance();
	post(exe.io_context(),[&]
	{
		start_coroutine([&]
		{
			for(auto &coro : coros)
			{
				coro_await(coro);
				coro_run_on_main();
			}
			exe.exit();
		},
		8192);
	});
	exe.exec();
}

template <typename Ins>
static coro_ptr call_exit(const rttr::method &method, Ins obj)
{
	return start_coroutine([&]
	{
		auto para_list = method.get_parameter_infos();
		if( para_list.empty() )
			method.invoke(obj);
	});
}

void plugin_call_handle::exit()
{
	std::list<coro_ptr> coros;
	for(auto &pair : registration::g_obj_hash)
	{
		auto &type = pair.first;
		auto &obj = pair.second;

		auto method = type.get_method(fmt::format("exit.{}", type.get_id()));
		if( method.is_valid() )
			coros.emplace_back(call_exit(method, obj));
		obj.clear();
	}
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( str_starts_with(method.get_name().to_string(), "gts.web.plugin.exit.") )
			coros.emplace_back(call_exit(method, rttr::instance()));
	}
	post(execution::instance().io_context(),[&]
	{
		start_coroutine([&]
		{
			for(auto &coro : coros)
			{
				coro_await(coro);
				coro_run_on_main();
			}
		},
		8192);
	});
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
	if( universal or not _new_connection_method_call(fmt::format("new_connection.{}", sock->local_endpoint().port()), sock) )
	{
		if( not _new_connection_method_call("new_connection", sock) )
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
		if( list.empty() )
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
			if( not method.invoke({}, static_cast<int>(other_args_vector.size()), other_args_vector.data()).to_bool() )
				return false;
		}
		else if( para.size() == 1 and para.begin()->get_type() == GTS_RTTR_TYPE(string_list) )
		{
			if( method.get_return_type() != GTS_RTTR_TYPE(bool) )
				continue ;
			ct++;
			if( not method.invoke({}, args).to_bool() )
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
		if( not str_starts_with(method.get_name().to_string(), method_name) or
			method.get_return_type() != GTS_RTTR_TYPE(std::string) or
			not method.get_parameter_infos().empty() )
			continue;

		auto str = method.invoke({}).to_string();
		if( str.empty() )
			continue;

		int j = static_cast<int>(str.size()) - 1;
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

GTS_NAMESPACE_END
