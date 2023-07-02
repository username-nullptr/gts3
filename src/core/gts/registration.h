#ifndef GTS_REGISTRATION_H
#define GTS_REGISTRATION_H

#include <gts/tcp_socket.h>
#include <gts/string_list.h>
#include <rttr/registration>
#include <fmt/format.h>

namespace gts
{

class registration
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(tcp_socket_ptr()))>
	registration(Func &&func, uint16_t port = 0) {
		rttr::registration::method(port == 0? "gts.plugin.new_connection" : fmt::format("gts.plugin.new_connection.{}", port), std::forward<Func>(func));
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func, const std::string &path = "")
	{
		rttr::registration::method("gts.plugin.init." + path, std::forward<Func>(func));
		return *this;
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func, const std::string &path = "")
	{
		rttr::registration::method("gts.plugin.init." + path, std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func, const std::string &path = "")
	{
		rttr::registration::method("gts.plugin.exit." + path, std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_same(decay_t<decltype(GTS_DECLVAL(Func)())>, std::string), int)>
	registration &view_status_method(Func &&func, const std::string &path = "")
	{
		rttr::registration::method("gts.plugin.view_status." + path, std::forward<Func>(func));
		return *this;
	}
};

namespace extension
{

class registration
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() {}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func, const std::string &path = "")
	{
		rttr::registration::method("gts.extension.plugin.init." + path, std::forward<Func>(func));
		return *this;
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func, const std::string &path = "")
	{
		rttr::registration::method("gts.extension.plugin.init." + path, std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func, const std::string &path = "")
	{
		rttr::registration::method("gts.extension.plugin.exit." + path, std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_same(decay_t<decltype(GTS_DECLVAL(Func)(0,GTS_DECLVAL(const char**)))>, bool), int)>
	registration &args_parsing_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.args_parsing", std::forward<Func>(func));
		return *this;
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_same(decay_t<decltype(GTS_DECLVAL(Func)(string_list()))>, bool), int), int U0=0>
	registration &args_parsing_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.args_parsing", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_same(decay_t<decltype(GTS_DECLVAL(Func)())>, std::string), int)>
	registration &view_version_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.view_version", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_same(decay_t<decltype(GTS_DECLVAL(Func)())>, std::string), int)>
	registration &view_help_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.view_help", std::forward<Func>(func));
		return *this;
	}
};

} //namespace extension

} //namespace gts

#ifndef GTS_REGISTRATION
# define GTS_REGISTRATION  RTTR_REGISTRATION
#endif //GTS_REGISTRATION


#endif //GTS_REGISTRATION_H
