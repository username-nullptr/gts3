#ifndef GTS_WEB_REGISTRATION_H
#define GTS_WEB_REGISTRATION_H

#include <gts/registration.h>
#include <gts/web/types.h>
#include <gts/http.h>
#include <gts/log.h>

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
	registration &init_method(Func &&func) {
		return reinterpret_cast<registration&>(gts::registration::init_method(std::forward<Func>(func)));
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(std::string())), int U0=0>
	registration &init_method(Func &&func) {
		return reinterpret_cast<registration&>(gts::registration::init_method(std::forward<Func>(func)));
	}

	template <typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)())>
	registration &exit_method(Func &&func) {
		return reinterpret_cast<registration&>(gts::registration::exit_method(std::forward<Func>(func)));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(decltype(GTS_DECLVAL(Func)()), std::string), int)>
	registration &view_status_method(Func &&func) {
		return reinterpret_cast<registration&>(gts::registration::view_status_method(std::forward<Func>(func)));
	}

public:
	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&)))>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), int8_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), uint8_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), int16_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), uint16_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), int32_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), uint32_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), int64_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

	template <int...http_method, typename Func, GTS_TYPE_DECLTYPE(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), uint64_t U0=0>
	registration &request_handle_method(const std::string &path, Func &&func) {
		return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
	}

public:
	//TODO websocket

public:
	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&))), bool), int)>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), bool), int), int8_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), bool), int), uint8_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), bool), int), int16_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), bool), int), uint16_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), bool), int), int32_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), bool), int), uint32_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), bool), int), int64_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
	}

	template <typename Func, GTS_TYPE_ENABLE_IF(gts_is_dsame(
			  decltype(GTS_DECLVAL(Func)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), bool), int), uint64_t U0=0>
	registration &filter_method(const std::string &path, Func &&func) {
		return _filter_method(path, std::forward<Func>(func));
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
	class class_ : public gts::registration::class_<Class>
	{
		GTS_DISABLE_COPY_MOVE(class_)

	public:
		explicit class_(const std::string &path = "") :
			m_path(path)
		{
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
			if( m_path[0] != '/' )
				m_path = "/" + m_path;
		}

	public:
		template <typename Return>
		class_ &init_method(Return(Class::*func)(void)) {
			return reinterpret_cast<class_&>(gts::registration::class_<Class>::init_method(func));
		}

		template <typename Return, typename Str, GTS_TYPE_DECLTYPE(GTS_CLASS_METHOD_DECLVAL(Class, Return, Str)(std::string()))>
		class_ &init_method(Return(Class::*func)(Str)) {
			return reinterpret_cast<class_&>(gts::registration::class_<Class>::init_method(func));
		}

		template <typename Return>
		class_ &exit_method(Return(Class::*func)(void)) {
			return reinterpret_cast<class_&>(gts::registration::class_<Class>::exit_method(func));
		}

		class_ &view_status_method(std::string(Class::*func)(void)) {
			return reinterpret_cast<class_&>(gts::registration::class_<Class>::view_status_method(func));
		}

	public:
		template <int...http_method, typename Return, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res)(GTS_DECLVAL(http::response&)))>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Res)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&)))>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Res)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Req)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int8_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Req)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Req, typename Res, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Res, Env)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), uint8_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Res, Env)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Res, typename Req, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Req, Env)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), int16_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Req, Env)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Res, typename Env, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Res, Env, Req)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), uint16_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Res, Env, Req)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Env, typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Env, Res, Req)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int32_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Env, Res, Req)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Env, typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Env, Req, Res)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), uint32_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Env, Req, Res)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

		template <int...http_method, typename Return, typename Req, typename Env, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, Return, Req, Env, Res)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), int64_t U0=0>
		class_ &request_handle_method(const std::string &path, Return(Class::*func)(Req, Env, Res)) {
			return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
		}

	public:
		//TODO websocket

	public:
		template <typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req)(GTS_DECLVAL(http::request&)))>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req)) {
			return _filter_method(path, func);
		}

		template <typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req, Res)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&)))>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req, Res)) {
			return _filter_method(path, func);
		}

		template <typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Res, Req)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int8_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Res, Req)) {
			return _filter_method(path, func);
		}

		template <typename Req, typename Res, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req, Res, Env)(GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&), environments())), uint8_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req, Res, Env)) {
			return _filter_method(path, func);
		}

		template <typename Res, typename Req, typename Env, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Res, Req, Env)(GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&), environments())), int16_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Res, Req, Env)) {
			return _filter_method(path, func);
		}

		template <typename Res, typename Env, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Res, Env, Req)(GTS_DECLVAL(http::response&), environments(), GTS_DECLVAL(http::request&))), uint16_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Res, Env, Req)) {
			return _filter_method(path, func);
		}

		template <typename Env, typename Res, typename Req, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Env, Res, Req)(environments(), GTS_DECLVAL(http::response&), GTS_DECLVAL(http::request&))), int32_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Env, Res, Req)) {
			return _filter_method(path, func);
		}

		template <typename Env, typename Req, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Env, Req, Res)(environments(), GTS_DECLVAL(http::request&), GTS_DECLVAL(http::response&))), uint32_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Env, Req, Res)) {
			return _filter_method(path, func);
		}

		template <typename Req, typename Env, typename Res, GTS_TYPE_DECLTYPE
				  (GTS_CLASS_METHOD_DECLVAL(Class, bool, Req, Env, Res)(GTS_DECLVAL(http::request&), environments(), GTS_DECLVAL(http::response&))), int64_t U0=0>
		class_ &filter_method(const std::string &path, bool(Class::*func)(Req, Env, Res)) {
			return _filter_method(path, func);
		}

	private:
		template <http::method...http_method, typename Func>
		class_ &_request_handle_method(std::string path, Func &&func)
		{
			if( not path.empty() )
			{
				auto n_it = std::unique(path.begin(), path.end(), [](char c0, char c1){
					return c0 == c1 and c0 == '/';
				});
				if( n_it != path.end() )
					path.erase(n_it, path.end());

				if( path[0] == '/' )
					path.erase(0,1);

				if( not path.empty() and path[path.size() - 1] == '/' )
					path.erase(path.size() - 1);
				path = m_path + path;
			}
			else
			{
				path = m_path;
				path.erase(path.size() - 1);
			}
			auto pair = g_path_hash.emplace(path, service_array());
			service_array_insert(path, pair.first->second, "new_request." + path + ".", std::forward<Func>(func), http_method...);
			return *this;
		}

		using service_array = registration::service_array;

		template <typename Func, typename...Tail>
		inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method, Tail&&...tail)
		{
			service_array_insert(path, method_array, method_name, std::forward<Func>(func), http_method);
			service_array_insert(path, method_array, method_name, std::forward<Func>(func), std::forward<Tail>(tail)...);
		}

		template <typename Func>
		inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method)
		{
			if( http_method < http::GET or http_method > http::TRACH )
				gts_log_fatal("service '{} ({})' registration: invalid method.", path, static_cast<int>(http_method));

			for(int i=http::GET; i<http::TRACH; i++)
			{
				if( http_method & i == 0 )
					continue;
				else if( method_array[i].method.is_valid() )
					gts_log_fatal("service '{} ({})' multiple registration.", path, http::method_string(static_cast<http::method>(i)));

				method_array[i].class_type = this->m_type;
				auto rmn = method_name + http::method_string(static_cast<http::method>(i));

				this->m_class_->method(rmn, std::forward<Func>(func));
				method_array[i].method = this->m_type.get_method(rmn);
			}
		}

	private:
		template <typename Func>
		inline class_ &_filter_method(std::string path, Func &&func)
		{
			if( path.empty() )
			{
				path = m_path;
				path.erase(path.size() - 1);
			}
			else
			{
				auto n_it = std::unique(path.begin(), path.end(), [](char c0, char c1){
					return c0 == c1 and c0 == '/';
				});
				if( n_it != path.end() )
					path.erase(n_it, path.end());

				if( path[0] != '/' )
					path = "/" + path;

				if( not path.empty() and path[path.size() - 1] == '/' )
					path.erase(path.size() - 1);
				path = m_path + path;
			}
			if( path[0] != '/' )
				path = "/" + path;

			auto pair = g_filter_path_map.emplace(path, service());
			if( pair.second == false )
				gts_log_fatal("service filter '{}' multiple registration.", path);

			auto method_name = "filter_method." + path;
			this->m_class_->method(method_name, std::forward<Func>(func));

			pair.first->second.class_type = this->m_type;
			pair.first->second.method = this->m_type.get_method(method_name);
			return *this;
		}

	protected:
		std::string m_path;
	};

private:
	template <http::method...http_method, typename Func>
	inline registration &_request_handle_method(std::string path, Func &&func)
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

			if( path[0] != '/' )
				path = "/" + path;

			if( path.size() > 1 and path[path.size() - 1] == '/' )
				path.erase(path.size() - 1);
		}
		auto pair = g_path_hash.emplace(path, service_array());
		service_array_insert(path, pair.first->second, "gts.web.plugin.new_request." + path + ".", std::forward<Func>(func), http_method...);
		return *this;
	}

	template <typename Func, typename...Tail>
	inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method, Tail&&...tail)
	{
		service_array_insert(path, method_array, method_name, std::forward<Func>(func), http_method);
		service_array_insert(path, method_array, method_name, std::forward<Func>(func), std::forward<Tail>(tail)...);
	}

	template <typename Func>
	inline void service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method)
	{
		if( http_method < http::GET or http_method > http::TRACH )
			gts_log_fatal("service '{} ({})' registration: invalid method.", path, static_cast<int>(http_method));

		for(int i=http::GET; i<http::TRACH; i++)
		{
			if( http_method & i == 0 )
				continue;
			if( method_array[i].method.is_valid() )
				gts_log_fatal("service '{} ({})' multiple registration.", path, http::method_string(static_cast<http::method>(i)));

			auto rmn = method_name + http::method_string(static_cast<http::method>(i));
			rttr::registration::method(rmn, std::forward<Func>(func));
			method_array[i].method = rttr::type::get_global_method(rmn);
		}
	}

private:
	template <typename Func>
	inline registration &_filter_method(std::string path, Func &&func)
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

			if( path[0] != '/' )
				path = "/" + path;
		}
		auto pair = g_filter_path_map.emplace(path, service());
		if( pair.second == false )
			gts_log_fatal("service filter '{}' multiple registration.", path);

		auto method_name = "gts.web.plugin.filter_method." + path;
		rttr::registration::method(method_name, std::forward<Func>(func));
		pair.first->second.method = rttr::type::get_global_method(method_name);
		return *this;
	}

private:
	static std::unordered_map<rttr::type, rttr::variant> &obj_hash() {
		return g_obj_hash;
	}

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

#define GTS_PLUGIN_HTTP_REQUEST_FILTER(_path, ...) \
	GTS_DECL_AUTO_FUNC(bool, __VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ gts::web::registration().filter_method(_path, GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC(bool, __VA_ARGS__)

#define GTS_PLUGIN_HTTP_REQUEST_HANDLE(_path, _method, ...) \
	GTS_DECL_AUTO_FUNC_VOID(__VA_ARGS__); \
	GTS_PLUGIN_REGISTRATION{ gts::web::registration().request_handle_method<_method>(_path, GTS_AUTO_FUNC_NAME); } \
	GTS_DECL_AUTO_FUNC_VOID(__VA_ARGS__)


#endif //GTS_WEB_REGISTRATION_H
