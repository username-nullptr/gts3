#include "plugins_service.h"
#include "gts/private/app_info.h"
#include "gts/web/global.h"

#include "service_io.h"
#include "gts/web/config_key.h"
#include "gts/algorithm.h"
#include "gts/settings.h"
#include "gts/log.h"

#include <nlohmann/json.hpp>
#include <rttr/library.h>
#include <cppfilesystem>
#include <iostream>

namespace gts { namespace web
{

using njson = nlohmann::json;

plugins_service::plugins_service(service_io &sio) :
	m_sio(sio)
{
	m_sio.socket.non_blocking(false);
}

bool plugins_service::exists()
{
	return check() != nullptr;
}

bool plugins_service::call()
{
	auto array = check();
	if( array == nullptr )
		return false;

	auto &ss = (*array)[registration::log2(m_sio.request().method())];
	if( not ss.method.is_valid() )
	{
		m_sio.return_to_null(http::hs_method_not_allowed);
		return true;
	}
	if( call_filter() )
		return true;
	else if( ss.class_type.is_valid() )
	{
		auto &obj = registration::obj_hash()[ss.class_type];
		class_method_call(ss.method, obj, GTS_RTTR_TYPE(http::response));
	}
	else
		global_method_call(ss.method, GTS_RTTR_TYPE(http::response));
	return true;
}

bool plugins_service::call_filter()
{
	auto lambda_call_filter = [this](registration::service &rs) -> bool
	{
		if( rs.method.get_return_type() != GTS_RTTR_TYPE(bool) )
			gts_log_fatal("*** Code bug !!! : service filter function return type error !!!");

		else if( rs.class_type.is_valid() )
		{
			auto &obj = registration::obj_hash()[rs.class_type];
			return class_method_call(rs.method, obj, GTS_RTTR_TYPE(http::request)).to_bool();
		}
		return global_method_call(rs.method, GTS_RTTR_TYPE(http::request)).to_bool();
	};
	for(auto &pair : registration::g_filter_path_map)
	{
		auto &path = pair.first;
		auto &rs = pair.second;

		if( not str_starts_with(m_sio.url_name, path) )
			continue ;

		else if( path[path.size() - 1] == '/' )
		{
			if( m_sio.url_name != path and lambda_call_filter(rs) )
				return true;
		}
		else if( lambda_call_filter(rs) )
			return true;
	}
	return false;
}

registration::service_array *plugins_service::check()
{
	if( m_sio.url_name.empty() )
		m_sio.url_name = "/";

	else if( m_sio.url_name.size() > 1 and m_sio.url_name[m_sio.url_name.size() - 1] == '/' )
		m_sio.url_name.pop_back();

	auto it = registration::g_path_hash.find(m_sio.url_name);
	if( it == registration::g_path_hash.end() )
		return nullptr;
	return &it->second;
}

static environments make_envs(service_io &sio)
{
	return environments{
		{ "REQUEST_METHOD"   , http::method_string(sio.request().method())           },
		{ "QUERY_STRING"     , sio.request().parameters_string()                     },
		{ "REMOTE_ADDR"      , sio.socket.remote_endpoint().address().to_string()    },
		{ "GATEWAY_INTERFACE", "RTTR/" RTTR_VERSION_STR                              },
		{ "SERVER_NAME"      , sio.socket.local_endpoint().address().to_string()     },
		{ "SERVER_PORT"      , fmt::format("{}", sio.socket.local_endpoint().port()) },
		{ "SERVER_PROTOCOL"  , "HTTP/" + sio.request().version()                     },
		{ "DOCUMENT_ROOT"    , resource_root()                                       },
		{ "SERVER_SOFTWARE"  , "GTS/1.0(GTS/" GTS_VERSION_STR ")"                    }
	};
}

rttr::variant plugins_service::global_method_call(const rttr::method &method, const rttr::type &p1_type)
{
	auto para_array = method.get_parameter_infos();
	if( para_array.size() == 1 )
	{
		if( para_array.begin()->get_type() == p1_type )
		{
			if( p1_type == GTS_RTTR_TYPE(http::request) )
				return method.invoke({}, m_sio.request());
			else if( p1_type == GTS_RTTR_TYPE(http::response) )
				return method.invoke({}, m_sio.response());
		}
	}
	else if( para_array.size() == 2 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) )
			return method.invoke({}, m_sio.request(), m_sio.response());

		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(http::request) )
			return method.invoke({}, m_sio.response(), m_sio.request());
	}
	else if( para_array.size() == 3 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = (it++)->get_type();
		auto t2 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(environments) )
			return method.invoke({}, m_sio.request(), m_sio.response(), make_envs(m_sio));

		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(environments) )
			return method.invoke({}, m_sio.response(), m_sio.request(), make_envs(m_sio));

		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::request) )
			return method.invoke({}, m_sio.response(), make_envs(m_sio), m_sio.request());

		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(http::request) )
			return method.invoke({}, make_envs(m_sio), m_sio.response(), m_sio.request());

		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(http::response) )
			return method.invoke({}, make_envs(m_sio), m_sio.request(), m_sio.response());

		else if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::response) )
			return method.invoke({}, m_sio.request(), make_envs(m_sio), m_sio.response());
	}
	gts_log_fatal("*** Code bug !!! : service function type error !!!");
	return {};
}

rttr::variant plugins_service::class_method_call(rttr::method &method, rttr::variant &obj, const rttr::type &p1_type)
{
	auto para_array = method.get_parameter_infos();
	if( para_array.size() == 1 )
	{
		if( para_array.begin()->get_type() == p1_type )
		{
			if( p1_type == GTS_RTTR_TYPE(http::request) )
				return method.invoke(obj, m_sio.request());
			else if( p1_type == GTS_RTTR_TYPE(http::response) )
				return method.invoke(obj, m_sio.response());
		}
	}
	else if( para_array.size() == 2 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) )
			return method.invoke(obj, m_sio.request(), m_sio.response());

		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(http::request) )
			return method.invoke(obj, m_sio.response(), m_sio.request());
	}
	else if( para_array.size() == 3 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = (it++)->get_type();
		auto t2 = it->get_type();

		if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(environments) )
			return method.invoke(obj, m_sio.request(), m_sio.response(), make_envs(m_sio));

		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(environments) )
			return method.invoke(obj, m_sio.response(), m_sio.request(), make_envs(m_sio));

		else if( t0 == GTS_RTTR_TYPE(http::response) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::request) )
			return method.invoke(obj, m_sio.response(), make_envs(m_sio), m_sio.request());

		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::response) and t2 == GTS_RTTR_TYPE(http::request) )
			return method.invoke(obj, make_envs(m_sio), m_sio.response(), m_sio.request());

		else if( t0 == GTS_RTTR_TYPE(environments) and t1 == GTS_RTTR_TYPE(http::request) and t2 == GTS_RTTR_TYPE(http::response) )
			return method.invoke(obj, make_envs(m_sio), m_sio.request(), m_sio.response());

		else if( t0 == GTS_RTTR_TYPE(http::request) and t1 == GTS_RTTR_TYPE(environments) and t2 == GTS_RTTR_TYPE(http::response) )
			return method.invoke(obj, m_sio.request(), make_envs(m_sio), m_sio.response());
	}
	gts_log_fatal("*** Code bug !!! : service function type error !!!");
	return {};
}

registration::service *plugins_service::find_filter(const std::string &url)
{
	auto it = registration::g_filter_path_map.lower_bound(url);
	if( it != registration::g_filter_path_map.begin() )
	{
		if( it != registration::g_filter_path_map.end() and it->first == url )
			return &it->second;

		--it;
		if( it->first == url.substr(0, it->first.size()) )
			return &it->second;
	}
	return nullptr;
}

}} //namespace gts::web
