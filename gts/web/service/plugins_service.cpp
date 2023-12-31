
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

#include "plugins_service.h"
#include "service_io.h"

#include <rttr/library.h>
#include <cppfilesystem>

GTS_WEB_NAMESPACE_BEGIN

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

	rttr::type p1_type = GTS_RTTR_TYPE(http::response);
	auto &headers = m_sio.request().headers();

	auto it = headers.find(http::header::connection);
	if( it != headers.end() and str_to_lower(it->second) == "upgrade" )
	{
		it = headers.find(http::header::upgrade);
		if( it != headers.end() and str_to_lower(it->second) == "websocket" )
			p1_type = GTS_RTTR_TYPE(web::socket_ptr);
	}
	if( ss.class_type.is_valid() )
		method_call(ss.method, registration::obj_hash()[ss.class_type], p1_type);
	else
		method_call(ss.method, {}, p1_type);
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
			return method_call(rs.method, obj, GTS_RTTR_TYPE(http::request)).to_bool();
		}
		return method_call(rs.method, rttr::instance(), GTS_RTTR_TYPE(http::request)).to_bool();
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
	return environments {
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

rttr::variant plugins_service::method_call(rttr::method &method, const rttr::instance &obj, const rttr::type &p1_type)
{
	auto para_array = method.get_parameter_infos();
	if( para_array.size() == 1 )
	{
		auto t0 = para_array.begin()->get_type();
		if( t0 == p1_type )
		{
			if( p1_type == GTS_RTTR_TYPE(http::request) )
				return method.invoke(obj, m_sio.request());
			else if( p1_type == GTS_RTTR_TYPE(http::response) )
				return method.invoke(obj, m_sio.response());
			else if( p1_type == GTS_RTTR_TYPE(web::socket_ptr) )
			{
				try {
					auto socket = make_websocket_ptr(m_sio.request(), m_sio.response());
					return method.invoke(obj, socket);
				}
				catch(...){
					return {};
				}
			}
		}
		else if( t0 == GTS_RTTR_TYPE(web::socket_ptr) )
			return {};
	}
	else if( para_array.size() == 2 )
	{
		auto it = para_array.begin();
		auto t0 = (it++)->get_type();
		auto t1 = it->get_type();

		if( p1_type == GTS_RTTR_TYPE(web::socket_ptr) )
		{
			try {
				auto socket = make_websocket_ptr(m_sio.request(), m_sio.response());

				if( t0 == GTS_RTTR_TYPE(web::socket_ptr) and t1 == GTS_RTTR_TYPE(web::environments) )
					return method.invoke(obj, socket, make_envs(m_sio));

				else if( t0 == GTS_RTTR_TYPE(web::environments) and t1 == GTS_RTTR_TYPE(web::socket_ptr) )
					return method.invoke(obj, make_envs(m_sio), socket);
			}
			catch(...){
				return {};
			}
		}
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

GTS_WEB_NAMESPACE_END
