#include "plugins_service.h"
#include "app_info.h"
#include "json.h"

#include "service/service_io.h"
#include "gts/web/registration.h"
#include "gts/web/config_key.h"
#include "gts/algorithm.h"
#include "gts/settings.h"
#include "gts/log.h"

#include <rttr/library.h>
#include <cppfilesystem>
#include <iostream>
#include <fstream>
#include <future>

namespace gts { namespace web
{

plugin_service::plugin_service(service_io &sio) :
	m_sio(sio)
{
	m_sio.socket->non_blocking(false);
}

static std::unordered_map<std::string, std::pair<rttr::type, rttr::variant>> g_plugin_obj_map;

void plugin_service::call()
{
	if( global_method_call(m_sio.url_name) or global_method_call("/" + m_sio.url_name) )
		return ;

	else if( m_sio.url_name[m_sio.url_name.size() - 1] != '/' )
	{
		if( global_method_call(m_sio.url_name + "/") or global_method_call("/" + m_sio.url_name + "/") )
			return ;
	}

	auto it = g_plugin_obj_map.find("gts.web.plugin.class./");
	if( it != g_plugin_obj_map.end() )
	{
		if( class_method_call(it->second.first, it->second.second, m_sio.url_name) or
			class_method_call(it->second.first, it->second.second, "/" + m_sio.url_name) )
			return ;

		else if( m_sio.url_name[m_sio.url_name.size() - 1] != '/' )
		{
			if( class_method_call(it->second.first, it->second.second, m_sio.url_name + "/") or
				class_method_call(it->second.first, it->second.second, "/" + m_sio.url_name + "/") )
				return ;
		}
	}

	auto str_list = string_split(m_sio.url_name, "/");
	assert(not str_list.empty());

	std::string class_name;
	for(;;)
	{
		class_name += "/" + str_list[0];
		str_list.pop_front();

		if( str_list.empty() )
			break;

		auto _url_name = str_list.join("/");
		auto &pair = find_plugin_obj(class_name);

		if( not pair.second.is_valid() )
			continue;

		else if( class_method_call(pair.first, pair.second, _url_name) or
				 class_method_call(pair.first, pair.second, "/" + _url_name) )
			return ;

		else if( m_sio.url_name[m_sio.url_name.size() - 1] != '/' )
		{
			if( class_method_call(pair.first, pair.second, _url_name + "/") or
				class_method_call(pair.first, pair.second, "/" + _url_name + "/") )
				return ;
		}
	}
	m_sio.return_to_null(http::hs_not_found);
}

static std::list<rttr::library> g_library_list;

#define _CHECK_RETURN_CALL(...) \
({ \
	if( method.get_return_type() == rttr::type::get<std::shared_ptr<std::future<void>>>() ) { \
		rttr::variant var = method.invoke(__VA_ARGS__); \
		futures.emplace_back(var.get_value<std::shared_ptr<std::future<void>>>()); \
	} \
	else \
		method.invoke(__VA_ARGS__); \
})

template <typename Ins>
static void call_init(rttr::method &method, Ins obj, std::list<std::shared_ptr<std::future<void>>> &futures)
{
	auto para_list = method.get_parameter_infos();
	if( para_list.empty() )
		_CHECK_RETURN_CALL(obj);

	else if( para_list.size() == 1 and para_list.begin()->get_type() == rttr::type::get<std::string>() )
		_CHECK_RETURN_CALL(obj, settings::global_instance().file_name());
}

template <typename Ins>
static void call_exit(rttr::method &method, Ins obj, std::list<std::shared_ptr<std::future<void>>> &futures)
{
	auto para_list = method.get_parameter_infos();
	if( para_list.empty() )
		_CHECK_RETURN_CALL(obj);
}

void plugin_service::init()
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
		std::string file_name;
		try {
			std::string file_path = it->value.GetString();
			if( not ends_with(file_path, "/") )
				file_path += "/";

			if( not appinfo::is_absolute_path(file_path) )
				file_path = json_file_path + file_path;

#ifdef _WINDOWS
			file_name = it->name.GetString() + std::string(".dll");
#elif defined(__unix__)
			file_name = std::string("lib") + it->name.GetString() + ".so";
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
			log_error("gts.web.plugin load failed: {}.", library.get_error_string());
	}

	std::list<std::shared_ptr<std::future<void>>> futures;

	for(auto method : rttr::type::get_global_methods())
	{
		if( starts_with(method.get_name().to_string(), "gts.web.plugin.init.") )
			call_init(method, rttr::instance(), futures);
	}
	for(auto &type : rttr::type::get_types())
	{
		auto name = type.get_name();
		if( not starts_with(name.to_string(), "gts.web.plugin.class.") )
			continue;

		auto obj = type.create();
		if( not obj.is_valid() )
			continue;

		g_plugin_obj_map.emplace(name.to_string(), std::make_pair(type, obj));
		auto method = type.get_method("init");

		if( method.is_valid() )
			call_init(method, obj, futures);
	}
	for(auto &future : futures)
		future->wait();
}

void plugin_service::exit()
{
	if( g_library_list.empty() )
		return ;
	std::list<std::shared_ptr<std::future<void>>> futures;

	for(auto &pair : g_plugin_obj_map)
	{
		auto method = pair.second.first.get_method("exit");
		if( method.is_valid() )
			call_exit(method, pair.second.second, futures);
		pair.second.second.clear();
	}
	for(auto method : rttr::type::get_global_methods())
	{
		if( starts_with(method.get_name().to_string(), "gts.web.plugin.exit.") )
			call_exit(method, rttr::instance(), futures);
	}

	for(auto &future : futures)
		future->wait();
	g_plugin_obj_map.clear();
}

std::string plugin_service::view_status()
{
	std::string result;
	for(auto &method : rttr::type::get_global_methods())
	{
		if( starts_with(method.get_name().to_string(), "gts.web.plugin.view_status.") and method.get_return_type() == rttr::type::get<std::string>() )
			result += method.invoke({}).get_value<std::string>();
	}
	if( not result.empty() and result[0] != '\n' )
		result = "\n" + result;
	return result;
}

static environments make_envs(service_io &sio)
{
	return environments{
		{ "REQUEST_METHOD"   , http::method_string(sio.request.method)                },
		{ "QUERY_STRING"     , sio.request.parameters_string                          },
		{ "REMOTE_ADDR"      , sio.socket->remote_endpoint().address().to_string()    },
		{ "GATEWAY_INTERFACE", "RTTR/" RTTR_VERSION_STR                               },
		{ "SERVER_NAME"      , sio.socket->local_endpoint().address().to_string()     },
		{ "SERVER_PORT"      , fmt::format("{}", sio.socket->local_endpoint().port()) },
		{ "SERVER_PROTOCOL"  , "HTTP/" + sio.request.version                          },
		{ "DOCUMENT_ROOT"    , service_io::resource_path()                            },
		{ "SERVER_SOFTWARE"  , "GTS/1.0(GTS/" GTS_VERSION_STR ")"                     }
	};
}

#define _CHECK_HTTP_METHOD_GLOBAL_CALL(...) \
({ \
	auto it = registration::g_gnrm_hash.find(_method_name); \
	assert(it != registration::g_gnrm_hash.end()); \
	if( it->second & m_sio.request.method ) \
		method.invoke({}, __VA_ARGS__); \
	else \
		m_sio.return_to_null(http::hs_method_not_allowed); \
})

bool plugin_service::global_method_call(const std::string &method_name)
{
	auto _method_name = "gts.web.plugin.new_request." + method_name;

	auto method = rttr::type::get_global_method
				  (_method_name, {rttr::type::get<tcp_socket_ptr>()});
	if( method.is_valid() )
	{
		_CHECK_HTTP_METHOD_GLOBAL_CALL(std::move(m_sio.socket));
		return true;
	}

	method = rttr::type::get_global_method
			 (_method_name, {rttr::type::get<tcp_socket_ptr>(), rttr::type::get<http::request>()});
	if( method.is_valid() )
	{
		_CHECK_HTTP_METHOD_GLOBAL_CALL(std::move(m_sio.socket), std::move(m_sio.request));
		return true;
	}

	method = rttr::type::get_global_method
			 (_method_name, {rttr::type::get<tcp_socket_ptr>(), rttr::type::get<http::request>(), rttr::type::get<environments>()});
	if( method.is_valid() )
	{
		_CHECK_HTTP_METHOD_GLOBAL_CALL(std::move(m_sio.socket), std::move(m_sio.request), make_envs(m_sio));
		return true;
	}
	return false;
}

#define _CHECK_HTTP_METHOD_CLASS_CALL(...) \
({ \
	auto it0 = registration::g_cmnrm_hash.find(_class_name); \
	assert(it0 != registration::g_cmnrm_hash.end()); \
	auto it1 = it0->second.find(_method_name); \
	assert(it1 != it0->second.end()); \
	if( it1->second & m_sio.request.method ) \
		method.invoke(obj, __VA_ARGS__); \
	else \
		m_sio.return_to_null(http::hs_method_not_allowed); \
})

bool plugin_service::class_method_call(rttr::type &type, rttr::variant &obj, const std::string &method_name)
{
	auto _class_name = type.get_name().to_string();
	auto _method_name = "new_request." + method_name;

	auto method = type.get_method(_method_name, {rttr::type::get<tcp_socket_ptr>()});
	if( method.is_valid() )
	{
		_CHECK_HTTP_METHOD_CLASS_CALL(std::move(m_sio.socket));
		return true;
	}

	method = type.get_method(_method_name, {rttr::type::get<tcp_socket_ptr>(), rttr::type::get<http::request>()});
	if( method.is_valid() )
	{
		_CHECK_HTTP_METHOD_CLASS_CALL(std::move(m_sio.socket), std::move(m_sio.request));
		return true;
	}

	method = type.get_method(_method_name, {rttr::type::get<tcp_socket_ptr>(), rttr::type::get<http::request>(), rttr::type::get<environments>()});
	if( method.is_valid() )
	{
		_CHECK_HTTP_METHOD_CLASS_CALL(std::move(m_sio.socket), std::move(m_sio.request), make_envs(m_sio));
		return true;
	}
	return false;
}

std::pair<rttr::type, rttr::variant> &plugin_service::find_plugin_obj(const std::string &class_name)
{
	auto _class_name = "gts.web.plugin.class." + class_name;

	auto it = g_plugin_obj_map.find(_class_name);
	if( it != g_plugin_obj_map.end() )
		return it->second;

	it = g_plugin_obj_map.find(_class_name + "/");
	if( it != g_plugin_obj_map.end() )
		return it->second;

	_class_name = "gts.web.plugin.class." + class_name.substr(1);

	it = g_plugin_obj_map.find(_class_name);
	if( it != g_plugin_obj_map.end() )
		return it->second;

	it = g_plugin_obj_map.find(_class_name + "/");
	if( it != g_plugin_obj_map.end() )
		return it->second;

	static auto invalid = std::make_pair(rttr::type::get<void>(), rttr::variant());
	return invalid;
}

}} //namespace gts::web
