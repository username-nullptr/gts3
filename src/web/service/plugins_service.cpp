#include "plugins_service.h"
#include "app_info.h"
#include "global.h"

#include "service/service_io.h"
#include "gts/web/registration.h"
#include "gts/web/config_key.h"
#include "gts/algorithm.h"
#include "gts/settings.h"
#include "gts/log.h"

#include <nlohmann/json.hpp>
#include <rttr/library.h>
#include <cppfilesystem>
#include <iostream>
#include <fstream>

namespace gts { namespace web
{

using njson = nlohmann::json;

plugin_service::plugin_service(service_io &sio) :
	m_sio(sio)
{
	m_sio.response.socket().non_blocking(false);
}

static std::unordered_map<rttr::type, rttr::variant> g_obj_hash;

void plugin_service::call()
{
	if( m_sio.url_name.empty() )
		m_sio.url_name = "/";

	else if( m_sio.url_name[m_sio.url_name.size() - 1] == '/' )
		m_sio.url_name.erase(m_sio.url_name.size() - 1);

	auto it = registration::g_path_hash.find(m_sio.url_name);
	if( it == registration::g_path_hash.end() )
		return m_sio.return_to_null(http::hs_not_found);

	auto &ss = it->second[m_sio.request.method()];
	if( not ss.method.is_valid() )
		return m_sio.return_to_null(http::hs_method_not_allowed);

	if( ss.class_type.is_valid() )
	{
		auto &obj = g_obj_hash[ss.class_type];
		return class_method_call(ss.method, obj);
	}
	global_method_call(ss.method);
}

static std::list<rttr::library> g_library_list;

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
static void call_init(rttr::method &method, Ins obj, std::list<future_ptr> &futures)
{
	auto para_list = method.get_parameter_infos();
	if( para_list.empty() )
		_CHECK_RETURN_CALL(obj);

	else if( para_list.size() == 1 and para_list.begin()->get_type() == GTS_RTTR_TYPE(std::string) )
		_CHECK_RETURN_CALL(obj, settings::global_instance().file_name());
}

template <typename Ins>
static void call_exit(rttr::method &method, Ins obj, std::list<future_ptr> &futures)
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
	if( not fs::exists(json_file) )
	{
		log_error("Web plugins json file is not exists.");
		return ;
	}

	std::ifstream file(json_file);
	auto json = njson::parse(file, nullptr, false);

	if( json.is_null() )
	{
		log_error("Web plugins json file read error.");
		return ;
	}
	auto json_file_path = file_path(json_file);

	for(auto &it : json.items())
	{
		std::string file_name;
		try {
			std::string file_path = it.value().get<std::string>();
			if( not ends_with(file_path, "/") )
				file_path += "/";

			if( not appinfo::is_absolute_path(file_path) )
				file_path = json_file_path + file_path;

#ifdef _WINDOWS
			file_name = it.key() + std::string(".dll");
#elif defined(__unix__)
			file_name = std::string("lib") + it.key() + ".so";
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
			g_library_list.pop_back(); //!!!library!!! -> null
		}
	}

	std::list<future_ptr> futures;

	for(auto method : rttr::type::get_global_methods())
	{
		if( starts_with(method.get_name().to_string(), "gts.web.plugin.init.") )
			call_init(method, rttr::instance(), futures);
	}

	for(auto &pair : registration::g_path_hash)
	{
		for(auto &ss : pair.second)
		{
			if( not ss.class_type.is_valid() )
				continue;

			auto pair = g_obj_hash.emplace(ss.class_type, rttr::variant());
			if( pair.second == false )
				continue ;

			pair.first->second = ss.class_type.create();
			auto &obj = pair.first->second;

			if( not obj.is_valid() )
				log_fatal("service class create failed.");

			auto method = ss.class_type.get_method(fmt::format("init.{}", ss.class_type.get_id()));
			if( method.is_valid() )
				call_init(method, obj, futures);
		}
	}
	for(auto &future : futures)
		future->wait();
}

void plugin_service::exit()
{
	std::list<future_ptr> futures;

	for(auto &pair : g_obj_hash)
	{
		auto &type = pair.first;
		auto &obj = pair.second;

		auto method = type.get_method(fmt::format("exit.{}", type.get_id()));
		if( method.is_valid() )
			call_exit(method, obj, futures);
		obj.clear();
	}
	for(auto method : rttr::type::get_global_methods())
	{
		if( starts_with(method.get_name().to_string(), "gts.web.plugin.exit.") )
			call_exit(method, rttr::instance(), futures);
	}
	for(auto &future : futures)
		future->wait();
	g_obj_hash.clear();
}

std::string plugin_service::view_status()
{
	std::string result;
	for(auto &method : rttr::type::get_global_methods())
	{
		if( starts_with(method.get_name().to_string(), "gts.web.plugin.view_status.") and method.get_return_type() == GTS_RTTR_TYPE(std::string) )
			result += method.invoke({}).get_value<std::string>();
	}
	for(auto &pair : g_obj_hash)
	{
		auto &type = pair.first;
		auto &obj = pair.second;

		auto method = type.get_method(fmt::format("view_status.{}", type.get_id()));
		if( method.is_valid() and method.get_return_type() == GTS_RTTR_TYPE(std::string) )
			result += method.invoke(obj).get_value<std::string>();
	}
	if( not result.empty() and result[0] != '\n' )
		result = "\n" + result;
	return result;
}

static environments make_envs(service_io &sio)
{
	return environments{
		{ "REQUEST_METHOD"   , http::method_string(sio.request.method())                        },
		{ "QUERY_STRING"     , sio.request.parameters_string()                                  },
		{ "REMOTE_ADDR"      , sio.response.socket().remote_endpoint().address().to_string()    },
		{ "GATEWAY_INTERFACE", "RTTR/" RTTR_VERSION_STR                                         },
		{ "SERVER_NAME"      , sio.response.socket().local_endpoint().address().to_string()     },
		{ "SERVER_PORT"      , fmt::format("{}", sio.response.socket().local_endpoint().port()) },
		{ "SERVER_PROTOCOL"  , "HTTP/" + sio.request.version()                                  },
		{ "DOCUMENT_ROOT"    , resource_root()                                                  },
		{ "SERVER_SOFTWARE"  , "GTS/1.0(GTS/" GTS_VERSION_STR ")"                               }
	};
}

void plugin_service::global_method_call(const rttr::method &method)
{
	auto para_array = method.get_parameter_infos();
	if( para_array.size() == 1 )
	{
		if( para_array.begin()->get_type() == GTS_RTTR_TYPE(http::response) )
			method.invoke({}, std::move(m_sio.response));
		return ;
	}
	else if( para_array.size() == 2 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) )
		{
			method.invoke({}, m_sio.request, std::move(m_sio.response));
			return ;
		}
		if( t0 == GTS_RTTR_TYPE(http::response) and t0 == GTS_RTTR_TYPE(http::request) )
		{
			method.invoke({}, m_sio.response, m_sio.request);
			return ;
		}
	}
	else if( para_array.size() == 3 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = (it++)->get_type();
		auto t2 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(environments) )
		{
			method.invoke({}, m_sio.request, std::move(m_sio.response), make_envs(m_sio));
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(environments) )
		{
			method.invoke({}, std::move(m_sio.response), m_sio.request, make_envs(m_sio));
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::request) )
		{
			method.invoke({}, std::move(m_sio.response), make_envs(m_sio), m_sio.request);
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(http::request) )
		{
			method.invoke({}, make_envs(m_sio), std::move(m_sio.response), m_sio.request);
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(http::response) )
		{
			method.invoke({}, make_envs(m_sio), m_sio.request, std::move(m_sio.response));
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::response) )
		{
			method.invoke({}, m_sio.request, make_envs(m_sio), std::move(m_sio.response));
			return ;
		}
	}
	log_fatal("*** Code bug !!! : service function type error !!!");
}

void plugin_service::class_method_call(rttr::method &method, rttr::variant &obj)
{
	auto para_array = method.get_parameter_infos();
	if( para_array.size() == 1 )
	{
		if( para_array.begin()->get_type() == GTS_RTTR_TYPE(http::response) )
		{
			method.invoke(obj, std::move(m_sio.response));
			return ;
		}
	}
	else if( para_array.size() == 2 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) )
		{
			method.invoke(obj, m_sio.request, std::move(m_sio.response));
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(http::request) )
		{
			method.invoke(obj, std::move(m_sio.response), m_sio.request);
			return ;
		}
	}
	else if( para_array.size() == 3 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = (it++)->get_type();
		auto t2 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(environments) )
		{
			method.invoke(obj, m_sio.request, std::move(m_sio.response), make_envs(m_sio));
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(environments) )
		{
			method.invoke(obj, std::move(m_sio.response), m_sio.request, make_envs(m_sio));
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::request) )
		{
			method.invoke(obj, std::move(m_sio.response), make_envs(m_sio), m_sio.request);
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(http::request) )
		{
			method.invoke(obj, make_envs(m_sio), std::move(m_sio.response), m_sio.request);
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(http::response) )
		{
			method.invoke(obj, make_envs(m_sio), m_sio.request, std::move(m_sio.response));
			return ;
		}
		else if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::response) )
		{
			method.invoke(obj, m_sio.request, make_envs(m_sio), std::move(m_sio.response));
			return ;
		}
	}
	log_fatal("*** Code bug !!! : service function type error !!!");
}

}} //namespace gts::web
