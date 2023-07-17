#include "server_tool.h"
#include "gts/algorithm.h"
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

void init(const std::string &config_file)
{
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( not starts_with(method.get_name().to_string(), "gts.plugin.init") )
			continue;
		auto list = method.get_parameter_infos();

		if( list.size() == 0 )
			method.invoke({});

		else if( list.size() == 1 and list.begin()->get_type() == GTS_RTTR_TYPE(std::string) )
			method.invoke({}, config_file);
	}
}

void exit()
{
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( starts_with(method.get_name().to_string(), "gts.plugin.exit") and method.get_parameter_infos().empty() )
			method.invoke({});
	}
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

bool new_connection(tcp_socket *sock, bool universal)
{
	if( universal or _new_connection_method_call(fmt::format("new_connection.{}", sock->local_endpoint().port()), sock) == false )
	{
		if( _new_connection_method_call("new_connection", sock) == false )
			return false;
	}
	return true;
}

std::string view_status()
{
	std::string result;
	auto it = rttr::type::get_global_methods().begin();

	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( starts_with(method.get_name().to_string(), "gts.plugin.view_status") and method.get_return_type() == GTS_RTTR_TYPE(std::string) )
			result = method.invoke({}).to_string();
	}
	return result;
}

} //namespace plugin_call

namespace extension { namespace plugin_call
{

void init(const std::string &config_file)
{
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( not starts_with(method.get_name().to_string(), "gts.extension.plugin.init.") )
			continue;

		auto list = method.get_parameter_infos();
		if( list.size() == 0 )
			method.invoke({});

		else if( list.size() == 1 and list.begin()->get_type() == GTS_RTTR_TYPE(std::string) )
			method.invoke({}, config_file);
	}
}

void exit()
{
	auto it = rttr::type::get_global_methods().begin();
	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( starts_with(method.get_name().to_string(), "gts.extension.plugin.exit.") and method.get_parameter_infos().empty() )
			method.invoke({});
	}
}

bool args_parsing(const string_list &args)
{
	std::size_t ct = 0;
	auto it = rttr::type::get_global_methods().begin();

	for(int i=0; it!=rttr::type::get_global_methods().end(); it=std::next(rttr::type::get_global_methods().begin(), ++i))
	{
		auto &method = *it;
		if( not starts_with(method.get_name().to_string(), "gts.extension.plugin.args_parsing.") )
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
		if( not starts_with(method.get_name().to_string(), method_name) )
			continue;

		else if( method.get_return_type() != GTS_RTTR_TYPE(std::string) or not method.get_parameter_infos().empty() )
			continue;

		result += method.invoke({}).to_string();
		if( result.empty() )
			return result;

		int j = result.size() - 1;
		for(; j>=0; j--)
		{
			if( result[j] != '\n' )
				break;
		}
		result = result.substr(0, j + 1) + "\n";
	}
	if( not result.empty() )
		result.pop_back();
	return result;
}

std::string view_version()
{
	return view_extension("gts.extension.plugin.view_version.");
}

std::string view_help()
{
	return view_extension("gts.extension.plugin.view_help.");
}

}} //namespace extension::plugin_call

} //namespace gts
