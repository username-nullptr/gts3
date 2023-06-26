#ifndef GTS_WEB_PLUGIN_INTERFACE_H
#define GTS_WEB_PLUGIN_INTERFACE_H

#include <rttr/registration>
#include <fmt/format.h>
#include <gts/http.h>
#include <gts/log.h>

namespace gts { namespace web
{

class plugin_service;

class registration
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() {}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &init_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.init.{}", typeid(Func).hash_code()), std::forward<Func>(func));
		return *this;
	}

	template <typename Func, typename T = decltype(std::declval<Func>()(std::string())), int U0=0>
	registration &init_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.init.{}", typeid(Func).hash_code()), std::forward<Func>(func));
		return *this;
	}

public:
	template <typename Func, typename T = decltype(std::declval<Func>()())>
	registration &exit_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.exit.{}", typeid(Func).hash_code()), std::forward<Func>(func));
		return *this;
	}

public:
	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(std::declval<http::response>()))>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(http::request(), std::declval<http::response>())), char U0=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(std::declval<http::response>(), http::request())), short U0=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(http::request(), std::declval<http::response>(), environments())), int U0=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(std::declval<http::response>(), http::request(), environments())), long U0=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(std::declval<http::response>(), environments(), http::request())), char U0=0, char U1=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(environments(), std::declval<http::response>(), http::request())), char U0=0, short U1=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(environments(), http::request(), std::declval<http::response>())), char U0=0, int U1=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, typename T = decltype(std::declval<Func>()(http::request(), environments(), std::declval<http::response>())), char U0=0, long U1=0>
	registration &new_request_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

public:
	template <typename Func, typename T = typename std::enable_if<
				  std::is_same<typename std::decay<decltype(std::declval<Func>()())>::type,
							   std::string>::value, int>::type>
	registration &view_status_method(Func &&func)
	{
		rttr::registration::method(fmt::format("gts.web.plugin.view_status.{}", typeid(Func).hash_code()), std::forward<Func>(func));
		return *this;
	}

private:
	struct service
	{
		rttr::type class_type {rttr::type::get_by_name("")};
		rttr::method method {rttr::type::get_global_method("")};
	};
	using service_array = std::array<service,8>;

	template <typename T>
	union addr_tlr
	{
		T ptr;
		std::uintptr_t addr;

		inline explicit addr_tlr(T ptr) : ptr(ptr) {}
		inline explicit addr_tlr(std::uintptr_t addr) : addr(addr) {}
	};

public:
	template <typename Class>
	class class_
	{
		GTS_DISABLE_COPY_MOVE(class_)

	public:
		explicit class_(const std::string &path = "") :
			m_path(path),
			m_class_name(fmt::format("gts.web.plugin.class.{}", typeid(Class).hash_code())),
			m_class_(std::make_shared<rttr::registration::class_<Class>>(m_class_name))
		{
			m_class_->constructor();
			m_type = rttr::type::get_by_name(m_class_name);

			if( m_path == "/" )
				return ;
			else if( m_path.empty() )
			{
				m_path = "/";
				return ;
			}
			auto n_it = std::unique(m_path.begin(), m_path.end(), [](char c0, char c1){
				return c0 == c1 and c0 == '/';
			});
			if( n_it != m_path.end() )
				m_path.erase(n_it, m_path.end());

			if( m_path[m_path.size() - 1] != '/' )
				m_path += '/';
			if( m_path[0] == '/' )
				m_path.erase(0,1);
		}

	public:
		template <typename Return>
		class_ &init_method(Return(Class::*func)(void))
		{
			m_class_->method(fmt::format("init.{}", m_type.get_id()), func);
			return *this;
		}

		template <typename Return, typename Str, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Str)>())(std::string()))>
		class_ &init_method(Return(Class::*func)(Str))
		{
			m_class_->method(fmt::format("init", m_type.get_id()), func);
			return *this;
		}

	public:
		template <typename Return>
		class_ &exit_method(Return(Class::*func)(void))
		{
			m_class_->method(fmt::format("exit", m_type.get_id()), func);
			return *this;
		}

	public:
		template <http::method...http_method, typename Return, typename Res, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Res)>())(std::declval<http::response>()))>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Res)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Req, typename Res, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Req, Res)>())(http::request(), std::declval<http::response>()))>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Req, Res)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Res, typename Req, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Res, Req)>())(std::declval<http::response>(), http::request())), char U0=0>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Res, Req)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Req, typename Res, typename Env, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Req, Res, Env)>())(http::request(), std::declval<http::response>(), environments())), short U0=0>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Req, Res, Env)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Res, typename Req, typename Env, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Res, Req, Env)>())(std::declval<http::response>(), http::request(), environments())), int U0=0>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Res, Req, Env)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Res, typename Env, typename Req, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Res, Env, Req)>())(std::declval<http::response>(), environments(), http::request())), long U0=0>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Res, Env, Req)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Env, typename Res, typename Req, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Env, Res, Req)>())(environments(), std::declval<http::response>(), http::request())), char U0=0, char U1=0>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Env, Res, Req)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Env, typename Req, typename Res, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Env, Req, Res)>())(environments(), http::request(), std::declval<http::response>())), char U0=0, short U1=0>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Env, Req, Res)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Req, typename Env, typename Res, typename T =
				  decltype((std::declval<Class>().*std::declval<Return(Class::*)(Req, Env, Res)>())(http::request(), environments(), std::declval<http::response>())), char U0=0, int U1=0>
		class_ &new_request_method(const std::string &path, Return(Class::*func)(Req, Env, Res)) {
			return _new_request_method<http_method...>(path, func);
		}

	private:
		template <http::method...http_method, typename Func>
		class_ &_new_request_method(std::string path, Func &&func)
		{
			if( not path.empty() )
			{
				auto n_it = std::unique(path.begin(), path.end(), [](char c0, char c1){
					return c0 == c1 and c0 == '/';
				});
				if( n_it != path.end() )
					path.erase(n_it, path.end());

				if( path.size() > 1 and path[0] == '/' )
					path.erase(0,1);
				if( path.size() > 1 and path[path.size() - 1] == '/' )
					path.erase(path.size() - 1);

				path = m_path + path;
			}
			else
			{
				path = m_path;
				path.erase(path.size() - 1);
			}
			if( path.size() > 1 and path[0] == '/' )
				path.erase(0,1);

			auto pair0 = registration::g_path_hash.emplace(path, service_array());
			if( pair0.second == false )
				log_fatal("service '{}' multiple registration.", path);

			addr_tlr at(func);
			std::string method_name = fmt::format("new_request.{}.", at.addr);

			auto pair1 = registration::g_id_hash.emplace(at.addr, method_name);
			if( pair1.second )
				m_class_->method(method_name, std::forward<Func>(func));

			service_array_insert(path, pair0.first->second, method_name, http_method...);
			return *this;
		}

		using service_array = registration::service_array;

		template <typename...Tail>
		inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, http::method http_method, Tail&&...tail)
		{
			service_array_insert(path, method_array, method_name, http_method);
			service_array_insert(path, method_array, method_name, std::forward<Tail>(tail)...);
		}

		inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, http::method http_method)
		{
			if( method_array[http_method].method.is_valid() )
				log_fatal("service '{} ({})' multiple registration.", path, http::method_string(http_method));
			else
			{
				method_array[http_method].class_type = m_type;
				method_array[http_method].method = m_type.get_method(method_name);
			}
		}

	private:
		std::string m_path;
		std::string m_class_name;
		std::shared_ptr<rttr::registration::class_<Class>> m_class_;
		rttr::type m_type {rttr::type::get_by_name("")};
	};

private:
	template <http::method...http_method, typename Func>
	inline registration &_new_request_method(std::string path, Func &&func)
	{
		if( path.empty() )
			path = "/";
		else
		{
			auto n_it = std::unique(path.begin(), path.end(), [](char c0, char c1){
				return c0 == c1 and c0 == '/';
			});
			if( n_it != path.end() )
				path.erase(n_it, path.end());

			if( path.size() > 1 and path[0] == '/' )
				path.erase(0,1);
			if( path.size() > 1 and path[path.size() - 1] == '/' )
				path.erase(path.size() - 1);
		}

		auto pair0 = g_path_hash.emplace(path, service_array());
		if( pair0.second == false )
			log_fatal("service '{}' multiple registration.", path);

		addr_tlr at(func);
		std::string method_name = fmt::format("gts.web.plugin.new_request.{}.", at.addr);

		auto pair1 = g_id_hash.emplace(at.addr, method_name);
		if( pair1.second )
			rttr::registration::method(method_name, std::forward<Func>(func));

		service_array_insert(path, pair0.first->second, method_name, http_method...);
		return *this;
	}

	template <typename...Tail>
	inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, http::method http_method, Tail&&...tail)
	{
		service_array_insert(path, method_array, method_name, http_method);
		service_array_insert(path, method_array, method_name, std::forward<Tail>(tail)...);
	}

	inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, http::method http_method)
	{
		if( method_array[http_method].method.is_valid() )
			log_fatal("service '{} ({})' multiple registration.", path, http::method_string(http_method));
		else
			method_array[http_method].method = rttr::type::get_global_method(method_name);
	}

private:
	static std::unordered_map<rttr::type::type_id, rttr::variant> g_obj_hash;
	static std::unordered_map<std::string, service_array> g_path_hash;
	static std::unordered_map<std::uintptr_t, std::string> g_id_hash;

private:
	template <typename Class> friend class class_;
	friend class plugin_service;
};

}} //namespace gts::web

#ifndef GTS_REGISTRATION
# define GTS_REGISTRATION  RTTR_REGISTRATION
#endif //GTS_REGISTRATION


#endif //GTS_WEB_PLUGIN_INTERFACE_H
