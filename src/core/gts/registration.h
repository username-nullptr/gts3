#ifndef GTS_REGISTRATION_H
#define GTS_REGISTRATION_H

#include <gts/tcp_socket.h>
#include <gts/string_list.h>
#include <rttr/registration>
#include <fmt/format.h>
#include <iostream>

namespace gts
{

class registration_base
{
	GTS_DISABLE_COPY_MOVE(registration_base)

public:
	registration_base() = default;
	virtual ~registration_base() = 0;

protected:
	template <typename This, typename Func>
	This &register_method(const std::string &prefix, const std::string &name, Func &&func)
	{
		if( g_func_set.emplace(reinterpret_cast<const void*>(&func)).second )
			rttr::registration::method(fmt::format("gts{}.plugin.{}.{}", prefix, name, g_gfs_counter++), std::forward<Func>(func));
		else
		{
			std::cerr << "*** Error: gts::registration_base::" << name << ": multiple registration." << std::endl;
			abort();
		}
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
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(tcp_socket_ptr&))), int U0=0>
	registration &new_connection(Func &&func, uint16_t port = 0) {
		return register_method<registration>("", port == 0? "gts.plugin.new_connection" : fmt::format("gts.plugin.new_connection.{}", port), std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func) {
		return register_method<registration>("", "init", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func) {
		return register_method<registration>("", "init", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func) {
		return register_method<registration>("", "exit", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_status_method(Func &&func) {
		return register_method<registration>("", "view_status", std::forward<Func>(func));
	}
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
		return register_method<registration>("extension", "init", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func) {
		return register_method<registration>("extension", "init", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func) {
		return register_method<registration>("extension", "exit", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)(0,GTS_DECLVAL(const char**))), bool), int)>
	registration &args_parsing_method(Func &&func) {
		return register_method<registration>("extension", "args_parsing", std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)(string_list())), bool), int), int U0=0>
	registration &args_parsing_method(Func &&func) {
		return register_method<registration>("extension", "args_parsing", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_version_method(Func &&func) {
		return register_method<registration>("extension", "view_version", std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_help_method(Func &&func) {
		return register_method<registration>("extension", "view_help", std::forward<Func>(func));
	}
};

} //namespace extension

} //namespace gts

#ifndef GTS_PLUGIN_REGISTRATION
# define GTS_PLUGIN_REGISTRATION  RTTR_PLUGIN_REGISTRATION
#endif //GTS_PLUGIN_REGISTRATION


#endif //GTS_REGISTRATION_H
