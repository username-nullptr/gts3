
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

#ifndef GTS_WEB_REGISTRATION_H
#define GTS_WEB_REGISTRATION_H

#include <gts/registration.h>
#include <gts/web/socket.h>

namespace gts
{

namespace web
{

class plugins_service;

class registration : public gts::registration
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() = default;

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func);

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func);

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_status_method(Func &&func);

public:
	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&)))>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), int8_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), uint8_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), int16_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), uint16_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), int32_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), uint32_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), int64_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), uint64_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func);

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&))), bool), int)>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), bool), int), int8_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), bool), int), uint8_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), bool), int), int16_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), bool), int), uint16_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), bool), int), int32_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), bool), int), uint32_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), bool), int), int64_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), bool), int), uint64_t U0=0>
	registration &filter_method(const std::string &path, Func &&func);

public:
	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(websocket_ptr()))>
	registration &new_websocket_connection(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(websocket_ptr(), environments())), int8_t U0=0>
	registration &new_websocket_connection(const std::string &path, Func &&func);

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(environments(), websocket_ptr())), uint8_t U0=0>
	registration &new_websocket_connection(const std::string &path, Func &&func);

private:
	struct service
	{
		rttr::type class_type {rttr::type::get_by_name("")};
		rttr::method method {rttr::type::get_global_method("")};
	};
	using service_array = std::array<service,8>;

public:
	template <typename Class>
	class class_ : public gts::registration::class_<Class>
	{
		GTS_DISABLE_COPY_MOVE(class_)

	public:
		explicit class_(const std::string &path = "");

	public:
		template <typename Return>
		class_ &init_method(Return(Class::*func)(void));

		template <typename Return, typename Str, GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Str)(std::string()))>
		class_ &init_method(Return(Class::*func)(Str));

		template <typename Return>
		class_ &exit_method(Return(Class::*func)(void));

		class_ &view_status_method(std::string(Class::*func)(void));

	public:
		template <int...http_method, typename Return, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res)(GTS_DECLVAL(http::response&)))>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res));

		template <int...http_method, typename Return, typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Res)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&)))>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Res));

		template <int...http_method, typename Return, typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Req)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int8_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Req));

		template <int...http_method, typename Return, typename Req, typename Res, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Res, Env)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), uint8_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Res, Env));

		template <int...http_method, typename Return, typename Res, typename Req, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Req, Env)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), int16_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Req, Env));

		template <int...http_method, typename Return, typename Res, typename Env, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Env, Req)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), uint16_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Env, Req));

		template <int...http_method, typename Return, typename Env, typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Env, Res, Req)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int32_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Env, Res, Req));

		template <int...http_method, typename Return, typename Env, typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Env, Req, Res)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), uint32_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Env, Req, Res));

		template <int...http_method, typename Return, typename Req, typename Env, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Env, Res)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), int64_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Env, Res));

	public:
		template <typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req)(GTS_DECLVAL(http::request&)))>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req));

		template <typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req, Res)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&)))>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req, Res));

		template <typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Res, Req)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int8_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Res, Req));

		template <typename Req, typename Res, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req, Res, Env)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), uint8_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req, Res, Env));

		template <typename Res, typename Req, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Res, Req, Env)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), int16_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Res, Req, Env));

		template <typename Res, typename Env, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Res, Env, Req)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), uint16_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Res, Env, Req));

		template <typename Env, typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Env, Res, Req)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int32_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Env, Res, Req));

		template <typename Env, typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Env, Req, Res)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), uint32_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Env, Req, Res));

		template <typename Req, typename Env, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req, Env, Res)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), int64_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req, Env, Res));

	public:
		template <int...http_method, typename Return, typename WebSockPtr, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, WebSockPtr)(websocket_ptr()))>
		class_ &new_websocket_connection(const std::string &path, Return(Class::*func)(WebSockPtr));

		template <int...http_method, typename Return, typename WebSockPtr, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, WebSockPtr, Env)(websocket_ptr(), environments())), int8_t U0=0>
		class_ &new_websocket_connection(const std::string &path, Return(Class::*func)(WebSockPtr, Env));

		template <int...http_method, typename Return, typename Env, typename WebSockPtr, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Env, WebSockPtr)(environments(), websocket_ptr())), uint8_t U0=0>
		class_ &new_websocket_connection(const std::string &path, Return(Class::*func)(Env, WebSockPtr));

	private:
		template <http::method...http_method, typename Func>
		class_ &_request_handle_method(std::string path, Func &&func);

		using service_array = registration::service_array;

		template <typename Func, typename...Tail>
		void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method, Tail&&...tail);

		template <typename Func>
		void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method);

		template <typename Func>
		class_ &_filter_method(std::string path, Func &&func);

	protected:
		std::string m_path;
	};

private:
	template <http::method...http_method, typename Func>
	registration &_request_handle_method(std::string path, Func &&func);

	template <typename Func, typename...Tail>
	void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method, Tail&&...tail);

	template <typename Func>
	void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method);

	template <typename Func>
	registration &_filter_method(std::string path, Func &&func);

private:
	static std::size_t log2(http::method n);
	static std::unordered_map<rttr::type, rttr::variant> &obj_hash();

private:
	static std::set<const void*> g_global_func_set;
	static std::size_t g_ggfs_counter;

private:
	static std::unordered_map<std::string, service_array> g_path_hash;
	static std::map<std::string, service> g_filter_path_map;
	template <typename Class> friend class class_;
	friend class plugins_service;
};

}} //namespace gts::web

#include <gts/web/detail/registration.h>

#define GTS_PLUGIN_HTTP_REQUEST_FILTER(_path, ...) \
	GTS_DECL_AUTO_FUNC(bool, __VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ gts::web::registration().filter_method(_path, GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC(bool, __VA_ARGS__)

#define GTS_PLUGIN_HTTP_REQUEST_HANDLE(_path, _method, ...) \
	GTS_DECL_AUTO_FUNC_VOID(__VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ gts::web::registration().request_handle_method<_method>(_path, GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC_VOID(__VA_ARGS__)


#endif //GTS_WEB_REGISTRATION_H
