#ifndef GTS_REGISTRATION_H
#define GTS_REGISTRATION_H

#include <gts/tcp_socket.h>
#include <gts/string_list.h>
#include <rttr/registration>
#include <gts/log.h>
#include <cppformat>
#include <future>

namespace gts
{

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

	template <typename This, typename Func, typename _U0 = is_function<Func>>
	This &register_method(const std::string &prefix, const std::string &name, Func &&func)
	{
		auto addr = reinterpret_cast<const void*>(&func);
		if( g_func_set.emplace(addr).second )
			rttr::registration::method(fmt::format("gts{}.plugin{}.{}", prefix, name, addr), std::forward<Func>(func));
		else
			gts_log_fatal("gts::registration_base::{}: multiple registration.", name);
		return *reinterpret_cast<This*>(this);
	}

protected:
	template <typename Func>
	using not_is_function = enable_if_t<not std::is_function<Func>::value, int>;

	template <typename This, typename Func, typename _U0 = not_is_function<Func>, typename _U1 = int>
	This &register_method(const std::string &prefix, const std::string &name, Func &&func)
	{
		rttr::registration::method(fmt::format("gts{}.plugin{}.co.{}", prefix, name, g_gfs_counter++), std::forward<Func>(func));
		return *reinterpret_cast<This*>(this);
	}

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
	registration &new_connection(Func &&func, uint16_t port = 0)
	{
		if( g_func_set.emplace(reinterpret_cast<const void*>(reinterpret_cast<const char*>(&func) + port)).second )
			rttr::registration::method(port == 0? "gts.plugin.new_connection" : fmt::format("gts.plugin.new_connection.{}", port), std::forward<Func>(func));
		else
			gts_log_fatal("gts::registration::new_connection: multiple registration.");
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func) {
		return register_method<registration>("", ".init", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func) {
		return register_method<registration>("", ".init", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func) {
		return register_method<registration>("", ".exit", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_status_method(Func &&func) {
		return register_method<registration>("", ".view_status", std::forward<Func>(func));
	}

public:
	template <typename Class>
	class class_
	{
		GTS_DISABLE_COPY_MOVE(class_)

	public:
		explicit class_() :
			m_class_name(fmt::format("gts.plugin.class.{}", typeid(Class).hash_code())),
			m_class_(std::make_shared<rttr::registration::class_<Class>>(m_class_name))
		{
			m_class_->constructor();
			m_type = rttr::type::get_by_name(m_class_name);
		}

	public:
		template <typename Return>
		class_ &init_method(Return(Class::*func)(void))
		{
			m_class_->method(fmt::format("init.{}", m_type.get_id()), func);
			return *this;
		}

		template <typename Return, typename Str, GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Str)(std::string()))>
		class_ &init_method(Return(Class::*func)(Str))
		{
			m_class_->method(fmt::format("init.{}", m_type.get_id()), func);
			return *this;
		}

		template <typename Return>
		class_ &exit_method(Return(Class::*func)(void))
		{
			m_class_->method(fmt::format("exit.{}", m_type.get_id()), func);
			return *this;
		}

		class_ &view_status_method(std::string(Class::*func)(void))
		{
			m_class_->method(fmt::format("view_status.{}", m_type.get_id()), func);
			return *this;
		}

	protected:
		std::string m_class_name;
		std::shared_ptr<rttr::registration::class_<Class>> m_class_;
		rttr::type m_type {rttr::type::get_by_name("")};
	};

protected:
	static std::unordered_map<rttr::type, rttr::variant> g_obj_hash;
	friend class plugin_call_handle;
};

namespace extension
{

class registration : public registration_base
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() = default;

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func) {
		return register_method<registration>(".extension", ".init", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func) {
		return register_method<registration>(".extension", ".init", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func) {
		return register_method<registration>(".extension", ".exit", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)(0,GTS_DECLVAL(const char**))), bool), int)>
	registration &args_parsing_method(Func &&func) {
		return register_method<registration>(".extension", ".args_parsing", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)(string_list())), bool), int), int U0=0>
	registration &args_parsing_method(Func &&func) {
		return register_method<registration>(".extension", ".args_parsing", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_version_method(Func &&func) {
		return register_method<registration>(".extension", ".view_version", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_help_method(Func &&func) {
		return register_method<registration>(".extension", ".view_help", std::forward<Func>(func));
	}
};

} //namespace extension

} //namespace gts

#define GTS_AUTO_XX_NAME(_prefix)  RTTR_CAT(_prefix, __LINE__)

#define GTS_AUTO_FUNC_NAME  GTS_AUTO_XX_NAME(__gts_auto_xx_name_)

#define GTS_REGISTRATION \
	static void GTS_AUTO_FUNC_NAME; \
	namespace { \
		struct GTS_AUTO_XX_NAME(__gts_auto_register_) { \
			GTS_AUTO_XX_NAME(gts_auto_register_)() { \
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
	GTS_DECL_EXPORT _return GTS_AUTO_FUNC_NAME(__VA_ARGS__)

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

#define GTS_PLUGIN_NEW_CONNECTION(socket) \
	GTS_PLUGIN_XX_FUNC_REG(gts::registration().new_connection, gts::tcp_socket_ptr &socket)

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

#define GTS_PLUGIN_CMD_INIT() \
	GTS_PLUGIN_XX_FUNC_REG(gts::extension::registration().init_method)

#define GTS_PLUGIN_CMD_EXIT() \
	GTS_PLUGIN_XX_FUNC_REG(gts::extension::registration().exit_method)

#define GTS_PLUGIN_CMD_ARGS_PARSING(...) \
	GTS_PLUGIN_XX_FUNC_REG(gts::extension::registration().args_parsing_method, __VA_ARGS__)

#define GTS_PLUGIN_CMD_VIEW_VERSION() \
	GTS_PLUGIN_XX_STR_FUNC_REG(gts::extension::registration().view_version_method)

#define GTS_PLUGIN_CMD_VIEW_HELP() \
	GTS_PLUGIN_XX_STR_FUNC_REG(gts::extension::registration().view_help_method)


#endif //GTS_REGISTRATION_H
