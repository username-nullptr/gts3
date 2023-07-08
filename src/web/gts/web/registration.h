#ifndef GTS_WEB_PLUGIN_INTERFACE_H
#define GTS_WEB_PLUGIN_INTERFACE_H

#include <rttr/registration>
#include <fmt/format.h>
#include <gts/http.h>
#include <gts/log.h>
#include <future>
#include <set>

namespace gts
{

typedef std::future<void>  future_void;

typedef std::shared_ptr<future_void>  future_ptr;

inline future_ptr make_future_ptr(future_void &&future)
{
	return std::make_shared<future_void>(std::move(future));
}

namespace web
{

class plugin_service;

class registration
{
	GTS_DISABLE_COPY_MOVE(registration)

public:
	registration() {}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &init_method(Func &&func)
	{
		if( g_global_func_set.emplace(reinterpret_cast<const void*>(&func)).second )
			rttr::registration::method(fmt::format("gts.web.plugin.init.{}", g_ggfs_counter++), std::forward<Func>(func));
		else
			log_fatal("registration::init_method: multiple registration.");
		return *this;
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func)
	{
		if( g_global_func_set.emplace(reinterpret_cast<const void*>(&func)).second )
			rttr::registration::method(fmt::format("gts.web.plugin.init.{}", g_ggfs_counter++), std::forward<Func>(func));
		else
			log_fatal("registration::init_method: multiple registration.");
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func)
	{
		if( g_global_func_set.emplace(reinterpret_cast<const void*>(&func)).second )
			rttr::registration::method(fmt::format("gts.web.plugin.exit.{}", g_ggfs_counter++), std::forward<Func>(func));
		else
			log_fatal("registration::exit_method: multiple registration.");
		return *this;
	}

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_same(decay_t<decltype(GTS_DECLVAL(Func)())>, std::string), int)>
	registration &view_status_method(Func &&func)
	{
		if( g_global_func_set.emplace(reinterpret_cast<const void*>(&func)).second )
			rttr::registration::method(fmt::format("gts.web.plugin.view_status.{}", g_ggfs_counter++), std::forward<Func>(func));
		else
			log_fatal("registration::view_status_method: multiple registration.");
		return *this;
	}

public:
	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response)))>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(http::request(), GTS_DECLVAL(http::response))), char U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response), http::request())), short U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(http::request(), GTS_DECLVAL(http::response), environments())), int U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response), http::request(), environments())), long U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response), environments(), http::request())), char U0=0, char U1=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::response), http::request())), char U0=0, short U1=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(environments(), http::request(), GTS_DECLVAL(http::response))), char U0=0, int U1=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

	template <http::method...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(http::request(), environments(), GTS_DECLVAL(http::response))), char U0=0, long U1=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _new_request_method<http_method...>(path, std::forward<Func>(func));
	}

private:
	struct service
	{
		rttr::type class_type {rttr::type::get_by_name("")};
		rttr::method method {rttr::type::get_global_method("")};
	};
	using service_array = std::array<service,8>;

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

		template <typename Return, typename Str, GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Str)(std::string()))>
		class_ &init_method(Return(Class::*func)(Str))
		{
			m_class_->method(fmt::format("init.{}", m_type.get_id()), func);
			return *this;
		}

	public:
		template <typename Return>
		class_ &exit_method(Return(Class::*func)(void))
		{
			m_class_->method(fmt::format("exit.{}", m_type.get_id()), func);
			return *this;
		}

	public:
		class_ &view_status_method(std::string(Class::*func)(void))
		{
			m_class_->method(fmt::format("view_status.{}", m_type.get_id()), func);
			return *this;
		}

	public:
		template <http::method...http_method, typename Return, typename Res,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Res)(GTS_DECLVAL(http::response)))>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Req, typename Res,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Res)(http::request(), GTS_DECLVAL(http::response)))>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Res)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Res, typename Req,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Req)(GTS_DECLVAL(http::response), http::request())), char U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Req)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Req, typename Res, typename Env,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Res, Env)(http::request(), GTS_DECLVAL(http::response), environments())), short U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Res, Env)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Res, typename Req, typename Env,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Req, Env)(GTS_DECLVAL(http::response), http::request(), environments())), int U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Req, Env)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Res, typename Env, typename Req,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Env, Req)(GTS_DECLVAL(http::response), environments(), http::request())), long U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Env, Req)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Env, typename Res, typename Req,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Env, Res, Req)(environments(), GTS_DECLVAL(http::response), http::request())), char U0=0, char U1=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Env, Res, Req)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Env, typename Req, typename Res,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Env, Req, Res)(environments(), http::request(), GTS_DECLVAL(http::response))), char U0=0, short U1=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Env, Req, Res)) {
			return _new_request_method<http_method...>(path, func);
		}

		template <http::method...http_method, typename Return, typename Req, typename Env, typename Res,
				  GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Env, Res)(http::request(), environments(), GTS_DECLVAL(http::response))), char U0=0, int U1=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Env, Res)) {
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
			service_array_insert(path, pair0.first->second, "new_request." + path + ".", std::forward<Func>(func), http_method...);
			return *this;
		}

		using service_array = registration::service_array;

		template <typename Func, typename...Tail>
		inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method http_method, Tail&&...tail)
		{
			service_array_insert(path, method_array, method_name, std::forward<Func>(func), http_method);
			service_array_insert(path, method_array, method_name, std::forward<Func>(func), std::forward<Tail>(tail)...);
		}

		template <typename Func>
		inline void service_array_insert(const std::string &path, service_array &method_array, std::string method_name, Func &&func, http::method http_method)
		{
			if( method_array[http_method].method.is_valid() )
				log_fatal("service '{} ({})' multiple registration.", path, http::method_string(http_method));
			else
			{
				method_array[http_method].class_type = m_type;
				method_name += http::method_string(http_method);
				m_class_->method(method_name, std::forward<Func>(func));
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
		service_array_insert(path, pair0.first->second, "gts.web.plugin.new_request." + path + ".", std::forward<Func>(func), http_method...);
		return *this;
	}

	template <typename Func, typename...Tail>
	inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method http_method, Tail&&...tail)
	{
		service_array_insert(path, method_array, method_name, std::forward<Func>(func), http_method);
		service_array_insert(path, method_array, method_name, std::forward<Func>(func), std::forward<Tail>(tail)...);
	}

	template <typename Func>
	inline void service_array_insert(const std::string &path, service_array &method_array, std::string method_name, Func &&func, http::method http_method)
	{
		if( method_array[http_method].method.is_valid() )
			log_fatal("service '{} ({})' multiple registration.", path, http::method_string(http_method));
		else
		{
			method_name += http::method_string(http_method);
			rttr::registration::method(method_name, std::forward<Func>(func));
			method_array[http_method].method = rttr::type::get_global_method(method_name);
		}
	}

private:
	static std::set<const void*> g_global_func_set;
	static std::size_t g_ggfs_counter;

private:
	static std::unordered_map<std::string, service_array> g_path_hash;
	template <typename Class> friend class class_;
	friend class plugin_service;
};

}} //namespace gts::web

#ifndef GTS_PLUGIN_REGISTRATION
# define GTS_PLUGIN_REGISTRATION  RTTR_PLUGIN_REGISTRATION
#endif //GTS_PLUGIN_REGISTRATION


#endif //GTS_WEB_PLUGIN_INTERFACE_H
