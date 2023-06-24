#include "server_tool.h"
#include <fmt/format.h>

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

namespace plugin_call
{

static rttr::variant g_plugin_obj;

static bool plugin_obj_check()
{
	if( g_plugin_obj.is_valid() )
		return true;

	auto type = rttr::type::get_by_name("gts.plugin.class");
	if( not type.is_valid() )
		return false;

	auto obj = type.create();
	if( not obj.is_valid() )
		return false;

	g_plugin_obj = obj;
	return true;
}

template <typename Ins>
static bool _init_method_call(rttr::method &method, Ins &&obj, const std::string &config_file)
{
	auto list = method.get_parameter_infos();
	if( list.size() == 0 )
	{
		method.invoke(obj);
		return true;
	}
	else if( list.size() == 1 and list.begin()->get_type() == rttr::type::get<std::string>() )
	{
		method.invoke(obj, config_file);
		return true;
	}
	return false;
}

void init(const std::string &config_file)
{
	auto method = rttr::type::get_global_method("gts.plugin.init");
	if( method.is_valid() and _init_method_call(method, rttr::instance(), config_file) )
		return ;
	else if( plugin_obj_check() == false )
		return ;

	method = g_plugin_obj.get_type().get_method("init");
	if( method.is_valid() and _init_method_call(method, g_plugin_obj, config_file) )
		return ;
}

void exit()
{
	auto method = rttr::type::get_global_method("gts.plugin.exit");
	if( method.is_valid() and method.get_parameter_infos().empty() )
	{
		method.invoke({});
		if( g_plugin_obj.is_valid() )
			g_plugin_obj.clear();
		return ;
	}
	else if( plugin_obj_check() == false )
		return ;

	method = g_plugin_obj.get_type().get_method("exit");
	if( method.is_valid() and method.get_parameter_infos().empty() )
		method.invoke(g_plugin_obj);
	g_plugin_obj.clear();
}

static bool _new_connection_method_call(const std::string &method_suffix, tcp_socket *sock)
{
	auto method = rttr::type::get_global_method("gts.plugin." + method_suffix, {rttr::type::get<tcp_socket_ptr>()});
	if( method.is_valid() )
	{
		method.invoke({}, std::shared_ptr<tcp_socket>(sock));
		return true;
	}
	else if( plugin_obj_check() )
	{
		method = g_plugin_obj.get_type().get_method(method_suffix, {rttr::type::get<std::shared_ptr<tcp_socket>>()});
		if( method.is_valid() )
		{
			method.invoke(g_plugin_obj, tcp_socket_ptr(sock));
			return true;
		}
	}
	return false;
}

bool new_connection(tcp_socket *sock, bool universal)
{
	if( universal or _new_connection_method_call(fmt::format("new_connection.{}", sock->local_endpoint().port()), sock) == false )
	{
		if( _new_connection_method_call("new_connection", sock) )
			return true;
	}
	return false;
}

std::string view_status()
{
	std::string result;
	auto method = rttr::type::get_global_method("gts.plugin.view_status");

	if( method.is_valid() and method.get_return_type() == rttr::type::get<std::string>() )
		result = method.invoke({}).to_string();

	else if( plugin_obj_check() )
	{
		method = g_plugin_obj.get_type().get_method("view_status");
		if( method.is_valid() and method.get_return_type() == rttr::type::get<std::string>() )
			result = method.invoke(g_plugin_obj).to_string();
	}
	return result;
}

} //namespace plugin_call

namespace extension { namespace plugin_call
{

void init(const std::string &config_file)
{
	auto method = rttr::type::get_global_method("gts.extension.plugin.init");
	if( method.is_valid() and method.get_parameter_infos().size() == 0 )
	{
		method.invoke({});
		return ;
	}
	method = rttr::type::get_global_method("gts.extension.plugin.init", {rttr::type::get<std::string>()});
	if( method.is_valid() )
		method.invoke({}, config_file);
}

void exit()
{
	auto method = rttr::type::get_global_method("gts.extension.plugin.exit");
	if( method.is_valid() and method.get_parameter_infos().size() == 0 )
		method.invoke({});
}

bool args_parsing(const string_list &args)
{
	auto method = rttr::type::get_global_method("gts.extension.plugin.args_parsing", {rttr::type::get<int>(), rttr::type::get<const char**>()});
	if( method.is_valid() )
	{
		if( method.get_return_type() == rttr::type::get<bool>() )
		{
			static auto other_args_vector = args.c_str_vector();
			return method.invoke({}, static_cast<int>(other_args_vector.size()), other_args_vector.data()).to_bool();
		}
	}
	method = rttr::type::get_global_method("gts.extension.plugin.args_parsing", {rttr::type::get<string_list>()});
	if( method.is_valid() and method.get_return_type() == rttr::type::get<bool>() )
		return method.invoke({}, args).to_bool();
	return false;
}

static std::string view_extension(const char *method_name)
{
	std::string result;
	auto method = rttr::type::get_global_method(method_name);

	if( method.is_valid() and method.get_parameter_infos().empty() and
		method.get_return_type() == rttr::type::get<std::string>() )
	{
		result = method.invoke({}).to_string();
		if( result.empty() )
			return result;

		int i = result.size() - 1;
		for(; i>=0; i--)
		{
			if( result[i] != '\n' )
				break;
		}
		result = result.substr(0, i + 1);
	}
	return result;
}

std::string view_version()
{
	auto result = view_extension("gts.extension.plugin.view_version");
	if( not result.empty() )
		result = "\nExtern:\n" + result;
	return result;
}

std::string view_help()
{
	auto result = view_extension("gts.extension.plugin.view_help");
	if( not result.empty() )
		result = "Extern:\n" + result;
	return result;
}

}} //namespace extension::plugin_call

} //namespace gts
