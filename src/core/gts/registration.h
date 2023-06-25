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
	template <typename Func, typename T = decltype(std::declval<Func>()(tcp_socket_ptr()))>
	registration(Func &&func, uint16_t port = 0) {
		rttr::registration::method(port == 0? "gts.plugin.new_connection" : fmt::format("gts.plugin.new_connection.{}", port), std::forward<Func>(func));
	}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &init_method(Func &&func)
	{
		rttr::registration::method("gts.plugin.init", std::forward<Func>(func));
		return *this;
	}

	template <typename Func, typename T = decltype(std::declval<Func>()(std::string())), int U0=0>
	registration &init_method(Func &&func)
	{
		rttr::registration::method("gts.plugin.init", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &exit_method(Func &&func)
	{
		rttr::registration::method("gts.plugin.exit", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = typename std::enable_if<
				  std::is_same<typename std::decay<decltype(std::declval<Func>()())>::type,
							   std::string>::value, int>::type>
	registration &view_status_method(Func &&func)
	{
		rttr::registration::method("gts.plugin.view_status", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Class>
	class class_
	{
		GTS_DISABLE_COPY_MOVE(class_)

	public:
		template <typename Return, typename Sock, typename T = decltype((std::declval<Class>().*std::declval<Return(Class::*)(Sock)>())(tcp_socket_ptr()))>
		class_(Return(Class::*func)(Sock), uint16_t port = 0) : m_class_(std::make_shared<rttr::registration::class_<Class>>("gts.plugin.class")) {
			m_class_->constructor().method(port == 0? "new_connection" : fmt::format("new_connection.{}", port), func);
		}

	public:
		template <typename Return>
		class_ &init_method(Return(Class::*func)(void))
		{
			m_class_->method("init", func);
			return *this;
		}

		template <typename Return, typename Str, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Str)>())(std::string()))>
		class_ &init_method(Return(Class::*func)(Str))
		{
			m_class_->method("init", func);
			return *this;
		}

	public:
		template <typename Return>
		class_ &exit_method(Return(Class::*func)(void))
		{
			m_class_->method("exit", func);
			return *this;
		}

	public:
		template <typename Func, typename T = typename std::enable_if<
					  std::is_same<typename std::decay<decltype((std::declval<Class>().*std::declval<Func>())())>::type,
								   std::string>::value, int>::type>
		class_ &view_status_method(Func &&func)
		{
			m_class_->method("view_status", func);
			return *this;
		}

	private:
		std::shared_ptr<rttr::registration::class_<Class>> m_class_;
	};
};

namespace extension
{

class registration
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() {}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &init_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.init", std::forward<Func>(func));
		return *this;
	}

	template <typename Func, typename T = decltype(std::declval<Func>()(std::string())), int U0=0>
	registration &init_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.init", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &exit_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.exit", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = typename std::enable_if<
				  std::is_same<typename std::decay<decltype(std::declval<Func>()(0,std::declval<const char**>()))>::type,
							   bool>::value, int>::type>
	registration &args_parsing_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.args_parsing", std::forward<Func>(func));
		return *this;
	}

	template <typename Func, typename T = typename std::enable_if<
				  std::is_same<typename std::decay<decltype(std::declval<Func>()(string_list()))>::type,
							   bool>::value, int>::type, int U0=0>
	registration &args_parsing_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.args_parsing", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = typename std::enable_if<
				  std::is_same<typename std::decay<decltype(std::declval<Func>()())>::type,
							   std::string>::value, int>::type>
	registration &view_version_method(Func &&func)
	{
		rttr::registration::method("gts.extension.plugin.view_version", std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = typename std::enable_if<
				  std::is_same<typename std::decay<decltype(std::declval<Func>()())>::type,
							   std::string>::value, int>::type>
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
