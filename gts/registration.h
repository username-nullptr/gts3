
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

#ifndef GTS_REGISTRATION_H
#define GTS_REGISTRATION_H

#include <gts/tcp_socket.h>
#include <gts/string_list.h>
#include <rttr/registration>
#include <cppformat>
#include <future>

GTS_NAMESPACE_BEGIN

using future_void = std::future<void>;

using future_ptr = std::shared_ptr<future_void>;

inline future_ptr make_future_ptr(future_void &&future) {
	return std::make_shared<future_void>(std::move(future));
}

class plugin_call_handle;

class registration_base
{
	GTS_DISABLE_COPY_MOVE(registration_base)

public:
	registration_base() = default;
	virtual ~registration_base() = 0;

protected:
	template <typename Func>
	using is_function = enable_if_t<std::is_function<Func>::value, int>;

	template <typename This, typename Func, typename UU0 = is_function<Func>>
	This &register_method(const std::string &prefix, const std::string &name, Func &&func);

protected:
	template <typename Func>
	using not_is_function = enable_if_t<not std::is_function<Func>::value, int>;

	template <typename This, typename Func, typename UU0 = not_is_function<Func>, typename UU1 = int>
	This &register_method(const std::string &prefix, const std::string &name, Func &&func);

protected:
	static std::set<const void*> g_func_set;
	static std::size_t g_gfs_counter;
};

class registration : public registration_base
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() = default;

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(tcp_socket_ptr&)))>
	registration &new_connection(Func &&func, uint16_t port = 0);

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
	template <typename Class>
	class class_
	{
		GTS_DISABLE_COPY_MOVE(class_)

	public:
		class_();

	public:
		template <typename Return>
		class_ &init_method(Return(Class::*func)());

		template <typename Return, typename Str, GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Str)(std::string()))>
		class_ &init_method(Return(Class::*func)(Str));

		template <typename Return>
		class_ &exit_method(Return(Class::*func)());

		class_ &view_status_method(std::string(Class::*func)());

	protected:
		std::string m_class_name;
		std::shared_ptr<rttr::registration::class_<Class>> m_class_;
		rttr::type m_type {rttr::type::get_by_name("")};
	};

protected:
	static std::unordered_map<rttr::type, rttr::variant> g_obj_hash;
	friend class plugin_call_handle;
};

GTS_NAMESPACE_END

GTS_EXTENSION_NAMESPACE_BEGIN

class registration : public registration_base
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() = default;

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func);

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func);

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func);

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)(0,GTS_DECLVAL(const char**))), bool), int)>
	registration &args_parsing_method(Func &&func);

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)(string_list())), bool), int), int U0=0>
	registration &args_parsing_method(Func &&func);

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_version_method(Func &&func);

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_help_method(Func &&func);
};

GTS_EXTENSION_NAMESPACE_END
#include <gts/detail/registration.h>

#define GTS_AUTO_XX_NAME(_prefix)  RTTR_CAT(_prefix, __LINE__)

#define GTS_AUTO_FUNC_NAME  GTS_AUTO_XX_NAME(__gts_auto_xx_name_)

#define GTS_REGISTRATION \
	static void GTS_AUTO_FUNC_NAME(); \
	namespace { \
		struct GTS_AUTO_XX_NAME(__gts_auto_register_) { \
			GTS_AUTO_XX_NAME(__gts_auto_register_)() { \
				GTS_AUTO_FUNC_NAME(); \
			} \
		}; \
	} \
	static const GTS_AUTO_XX_NAME(__gts_auto_register_) GTS_AUTO_XX_NAME(__auto_register_); \
	static void GTS_AUTO_FUNC_NAME()

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

#if RTTR_COMPILER == RTTR_COMPILER_MSVC
# define GTS_PLUGIN_REGISTRATION GTS_REGISTRATION
#else //compiler
# define GTS_PLUGIN_REGISTRATION \
	RTTR_DECLARE_PLUGIN_DTOR static void GTS_AUTO_XX_NAME(__gts_auto_unregister_reflection_function_)() { \
		rttr::detail::get_registration_manager().unregister(); \
	} \
	RTTR_DECLARE_PLUGIN_CTOR static void GTS_AUTO_XX_NAME(__gts_auto_register_reflection_function_)()
#endif //compiler

#define GTS_DECL_AUTO_FUNC(_return, ...) \
	static _return GTS_AUTO_FUNC_NAME(__VA_ARGS__)

#define GTS_DECL_AUTO_FUNC_VOID(...) \
	GTS_DECL_AUTO_FUNC(void, ##__VA_ARGS__)

#define GTS_DECL_AUTO_FUNC_STRING(...) \
	GTS_DECL_AUTO_FUNC(std::string, ##__VA_ARGS__)

#define GTS_PLUGIN_XX_FUNC_REG(_method, ...) \
	GTS_DECL_AUTO_FUNC_VOID(__VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ _method(GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC_VOID(__VA_ARGS__)

#define GTS_PLUGIN_XX_STR_FUNC_REG(_method, ...) \
	GTS_DECL_AUTO_FUNC_STRING(__VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ _method(GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC_STRING(__VA_ARGS__)

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

#define GTS_PLUGIN_INIT(...) \
	GTS_PLUGIN_XX_FUNC_REG(gts::registration().init_method, ##__VA_ARGS__)

#define GTS_PLUGIN_ASYNC_INIT(...) \
	GTS_DECL_AUTO_FUNC(gts::future_ptr, ##__VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ gts::registration().init_method(GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC(gts::future_ptr, ##__VA_ARGS__)

#define GTS_PLUGIN_EXIT() \
	GTS_PLUGIN_XX_FUNC_REG(gts::registration().exit_method)

#define GTS_PLUGIN_ASYNC_EXIT() \
	GTS_DECL_AUTO_FUNC(gts::future_ptr); \
	GTS_PLUGIN_REGISTRATION{ gts::registration().exit_method(GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC(gts::future_ptr)

#define GTS_PLUGIN_VIEW_STATUS() \
	GTS_PLUGIN_XX_STR_FUNC_REG(gts::registration().view_status_method)

#define GTS_PLUGIN_NEW_CONNECTION(_socket, ...) \
	GTS_DECL_AUTO_FUNC_VOID(gts::tcp_socket_ptr&); \
	GTS_PLUGIN_REGISTRATION{ gts::registration().new_connection(GTS_AUTO_FUNC_NAME, ##__VA_ARGS__); } \
	GTS_DECL_AUTO_FUNC_VOID(gts::tcp_socket_ptr &_socket)

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

#define GTS_PLUGIN_CMD_INIT() \
	GTS_PLUGIN_XX_FUNC_REG(gts::extension::registration().init_method)

#define GTS_PLUGIN_CMD_EXIT() \
	GTS_PLUGIN_XX_FUNC_REG(gts::extension::registration().exit_method)

#define GTS_PLUGIN_CMD_ARGS_PARSING(...) \
	GTS_DECL_AUTO_FUNC(bool, __VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ gts::extension::registration().args_parsing_method(GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC(bool, __VA_ARGS__)

#define GTS_PLUGIN_CMD_VIEW_VERSION() \
	GTS_PLUGIN_XX_STR_FUNC_REG(gts::extension::registration().view_version_method)

#define GTS_PLUGIN_CMD_VIEW_HELP() \
	GTS_PLUGIN_XX_STR_FUNC_REG(gts::extension::registration().view_help_method)


#endif //GTS_REGISTRATION_H
