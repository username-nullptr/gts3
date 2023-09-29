#ifndef GTS_WEB_DETAIL_REGISTRATION_H
#define GTS_WEB_DETAIL_REGISTRATION_H

namespace gts { namespace web
{

template <typename Func, typename _GTD_0>
registration &registration::init_method(Func &&func)
{
	return reinterpret_cast<registration&>(gts::registration::init_method(std::forward<Func>(func)));
}

template <typename Func, typename _GTD_0, int U0>
registration &registration::init_method(Func &&func)
{
	return reinterpret_cast<registration&>(gts::registration::init_method(std::forward<Func>(func)));
}

template <typename Func, typename _GTD_0>
registration &registration::exit_method(Func &&func)
{
	return reinterpret_cast<registration&>(gts::registration::exit_method(std::forward<Func>(func)));
}

template <typename Func, typename _GTEI_0>
registration &registration::view_status_method(Func &&func)
{
	return reinterpret_cast<registration&>(gts::registration::view_status_method(std::forward<Func>(func)));
}

template <int...http_method, typename Func, typename _GTD_0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, int8_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, uint8_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, int16_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, uint16_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, int32_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, uint32_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, int64_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <int...http_method, typename Func, typename _GTD_0, uint64_t U0>
registration &registration::request_handle_method(const std::string &path, Func &&func)
{
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, int8_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, uint8_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, int16_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, uint16_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, int32_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, uint32_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, int64_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <typename Func, typename _GTEI_0, uint64_t U0>
registration &registration::filter_method(const std::string &path, Func &&func)
{
	return _filter_method(path, std::forward<Func>(func));
}

template <http::method...http_method, typename Func>
registration &registration::_request_handle_method(std::string path, Func &&func)
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
void registration::service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method, Tail&&...tail)
{
	service_array_insert(path, method_array, method_name, std::forward<Func>(func), http_method);
	service_array_insert(path, method_array, method_name, std::forward<Func>(func), std::forward<Tail>(tail)...);
}

template <typename Func>
void registration::service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method)
{
	if( http_method < http::GET or http_method > http::TRACH )
		gts_log_fatal("service '{} ({})' registration: invalid method.", path, static_cast<int>(http_method));

	for(int i=http::GET; i<=http::TRACH; i<<=1)
	{
		if( (http_method & i) == 0 )
			continue;

		auto hc = log2(static_cast<http::method>(i));
		if( method_array[hc].method.is_valid() )
			gts_log_fatal("service '{} ({})' multiple registration.", path, http::method_string(static_cast<http::method>(i)));

		auto rmn = method_name + http::method_string(static_cast<http::method>(i));
		rttr::registration::method(rmn, std::forward<Func>(func));
		method_array[hc].method = rttr::type::get_global_method(rmn);
	}
}

template <typename Func>
registration &registration::_filter_method(std::string path, Func &&func)
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

template <typename Class>
registration::class_<Class>::class_(const std::string &path) :
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

template <typename Class>
template <typename Return>
registration::class_<Class> &registration::class_<Class>::init_method(Return(Class::*func)(void)) {
	return reinterpret_cast<class_&>(gts::registration::class_<Class>::init_method(func));
}

template <typename Class>
template <typename Return, typename Str, typename _GTD_0>
registration::class_<Class> &registration::class_<Class>::init_method(Return(Class::*func)(Str)) {
	return reinterpret_cast<class_&>(gts::registration::class_<Class>::init_method(func));
}

template <typename Class>
template <typename Return>
registration::class_<Class> &registration::class_<Class>::exit_method(Return(Class::*func)(void)) {
	return reinterpret_cast<class_&>(gts::registration::class_<Class>::exit_method(func));
}

template <typename Class>
registration::class_<Class> &registration::class_<Class>::view_status_method(std::string(Class::*func)(void)) {
	return reinterpret_cast<class_&>(gts::registration::class_<Class>::view_status_method(func));
}

template <typename Class>
template <int...http_method, typename Return, typename Res, typename _GTD_0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Res)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Req, typename Res, typename _GTD_0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Req, Res)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Res, typename Req, typename _GTD_0, int8_t U0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Res, Req)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Req, typename Res, typename Env, typename _GTD_0, uint8_t U0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Req, Res, Env)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Res, typename Req, typename Env, typename _GTD_0, int16_t U0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Res, Req, Env)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Res, typename Env, typename Req, typename _GTD_0, uint16_t U0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Res, Env, Req)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Env, typename Res, typename Req, typename _GTD_0, int32_t U0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Env, Res, Req)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Env, typename Req, typename Res, typename _GTD_0, uint32_t U0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Env, Req, Res)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <int...http_method, typename Return, typename Req, typename Env, typename Res, typename _GTD_0, int64_t U0>
registration::class_<Class> &registration::class_<Class>::request_handle_method(const std::string &path, Return(Class::*func)(Req, Env, Res)) {
	return _request_handle_method<static_cast<http::method>(http_method)...>(path, func);
}

template <typename Class>
template <typename Req, typename _GTD_0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Req)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Req, typename Res, typename _GTD_0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Req, Res)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Res, typename Req, typename _GTD_0, int8_t U0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Res, Req)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Req, typename Res, typename Env, typename _GTD_0, uint8_t U0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Req, Res, Env)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Res, typename Req, typename Env, typename _GTD_0, int16_t U0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Res, Req, Env)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Res, typename Env, typename Req, typename _GTD_0, uint16_t U0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Res, Env, Req)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Env, typename Res, typename Req, typename _GTD_0, int32_t U0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Env, Res, Req)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Env, typename Req, typename Res, typename _GTD_0, uint32_t U0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Env, Req, Res)) {
	return _filter_method(path, func);
}

template <typename Class>
template <typename Req, typename Env, typename Res, typename _GTD_0, int64_t U0>
registration::class_<Class> &registration::class_<Class>::filter_method(const std::string &path, bool(Class::*func)(Req, Env, Res)) {
	return _filter_method(path, func);
}

template <typename Class>
template <http::method...http_method, typename Func>
registration::class_<Class> &registration::class_<Class>::_request_handle_method(std::string path, Func &&func)
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

template <typename Class>
template <typename Func, typename...Tail>
void registration::class_<Class>::service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method, Tail&&...tail)
{
	service_array_insert(path, method_array, method_name, std::forward<Func>(func), http_method);
	service_array_insert(path, method_array, method_name, std::forward<Func>(func), std::forward<Tail>(tail)...);
}

template <typename Class>
template <typename Func>
void registration::class_<Class>::service_array_insert(const std::string &path, service_array &method_array, const std::string &method_name, Func &&func, http::method_flags http_method)
{
	if( http_method < http::GET or http_method > http::TRACH )
		gts_log_fatal("service '{} ({})' registration: invalid method.", path, static_cast<int>(http_method));

	for(int i=http::GET; i<=http::TRACH; i<<=1)
	{
		if( (http_method & i) == 0 )
			continue;

		auto hc = log2(static_cast<http::method>(i));
		if( method_array[hc].method.is_valid() )
			gts_log_fatal("service '{} ({})' multiple registration.", path, http::method_string(static_cast<http::method>(i)));

		method_array[hc].class_type = this->m_type;
		auto rmn = method_name + http::method_string(static_cast<http::method>(i));

		this->m_class_->method(rmn, std::forward<Func>(func));
		method_array[hc].method = this->m_type.get_method(rmn);
	}
}

template <typename Class>
template <typename Func>
registration::class_<Class> &registration::class_<Class>::_filter_method(std::string path, Func &&func)
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

inline std::unordered_map<rttr::type, rttr::variant> &registration::obj_hash()
{
	return g_obj_hash;
}

}} //namespace gts::web


#endif //GTS_WEB_DETAIL_REGISTRATION_H
