#ifndef GTS_WEB_PLUGIN_INTERFACE_H
#define GTS_WEB_PLUGIN_INTERFACE_H

#include <fmt/format.h>
#include <rttr/registration>
#include <gts/http/request.h>
#include <gts/web_global.h>
#include <gts/tcp_socket.h>

namespace gts { namespace web
{

class plugin_service;

class registration
{
public:
	registration() : m_sid(g_sid++) {}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &init_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.init.{}", m_sid), std::forward<Func>(func));
		return *this;
	}

	template <typename Func, typename T = decltype(std::declval<Func>()(std::string())), int U0=0>
	registration &init_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.init.{}", m_sid), std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &exit_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.exit.{}", m_sid), std::forward<Func>(func));
		return *this;
	}

public:
	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(tcp_socket_ptr()))>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(tcp_socket_ptr(), http::request())), int U0=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(tcp_socket_ptr(), http::request(), environments())), char U0=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

public:
	template <typename Func, typename T = typename std::enable_if<
				  std::is_same<typename std::decay<decltype(std::declval<Func>()())>::type,
							   std::string>::value, int>::type>
	registration &view_status_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.view_status.{}", m_sid), std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Class>
	class class_
	{
	public:
		explicit class_(const std::string &path) :
			m_class_name("gts.web.plugin.class." + path),
			m_class_(std::make_shared<rttr::registration::class_<Class>>(m_class_name))
		{
			m_class_->constructor();
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
		template <http::method...http_method, typename Return, typename Sock, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Sock)>())(tcp_socket_ptr()))>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Sock)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Sock, typename Req, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Sock, Req)>())(tcp_socket_ptr(), http::request()))>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Sock, Req)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Sock, typename Req, typename Env, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Sock, Req, Env)>())(tcp_socket_ptr(), http::request(), environments()))>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Sock, Req, Env)) {
			return _new_request_method<http_method...>(path, func);
		}

	private:
		template <http::method...http_method, typename Func>
		class_ &_new_request_method(const std::string &path, Func &&func)
		{
			auto method_name = "new_request." + path;
			m_class_->method(method_name, std::forward<Func>(func));

			auto it0 = registration::g_cmnrm_hash.find(m_class_name);
			if( it0 != registration::g_cmnrm_hash.end() )
			{
				auto it1 = it0->second.find(method_name);
				if( it1 != it0->second.end() )
					return *this;
			}
			cmnrm_hash_insert(method_name, http_method...);
			return *this;
		}

		template <typename...Tail>
		void cmnrm_hash_insert(const std::string &method_name, http::method http_method, Tail&&...tail)
		{
			cmnrm_hash_insert(method_name, http_method);
			cmnrm_hash_insert(method_name, std::forward<Tail>(tail)...);
		}

		void cmnrm_hash_insert(const std::string &method_name, http::method http_method)
		{
			auto pair0 = registration::g_cmnrm_hash.emplace(m_class_name, std::unordered_map<std::string, std::size_t>{{method_name, http_method}});
			if( pair0.second )
				return ;

			auto pair1 = pair0.first->second.emplace(method_name, http_method);
			if( pair1.second == false )
				pair1.first->second |= http_method;
		}

	private:
		std::string m_class_name;
		std::shared_ptr<rttr::registration::class_<Class>> m_class_;
	};

private:
	template <http::method...http_method, typename Func>
	inline registration &_new_request_method(const std::string &path, Func &&func)
	{
		auto method_name = "gts.web.plugin.new_request." + path;
		rttr::registration::method(method_name, std::forward<Func>(func));
		if( g_gnrm_hash.find(method_name) == g_gnrm_hash.end() )
			gnrm_hash_insert(method_name, http_method...);
		return *this;
	}

	template <typename...Tail>
	void gnrm_hash_insert(const std::string &method_name, http::method http_method, Tail&&...tail)
	{
		gnrm_hash_insert(method_name, http_method);
		gnrm_hash_insert(method_name, std::forward<Tail>(tail)...);
	}

	void gnrm_hash_insert(const std::string &method_name, http::method http_method)
	{
		auto pair = g_gnrm_hash.emplace(method_name, http_method);
		if( pair.second == false )
			pair.first->second |= http_method;
	}

private:
	std::size_t m_sid = 0;
	static std::size_t g_sid;
	static std::unordered_map<std::string, std::size_t> g_gnrm_hash;
	static std::unordered_map<std::string, std::unordered_map<std::string, std::size_t>> g_cmnrm_hash;

private:
	template <typename Class> friend class class_;
	friend class plugin_service;
};

}} //namespace gts::web


#endif //GTS_WEB_PLUGIN_INTERFACE_H
