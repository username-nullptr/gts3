#ifndef GTS_HTTP_DETAIL_REQUEST_H
#define GTS_HTTP_DETAIL_REQUEST_H

namespace gts { namespace http
{

template <class Sesn>
std::shared_ptr<Sesn> request::session() const
{
	static_assert(gts_is_base_of(http::session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");

	using sesn_ptr = std::shared_ptr<Sesn>;
	auto it = cookies().find("session_id");

	if( it == cookies().end() )
		return sesn_ptr();

	auto ptr = http::session::get<Sesn>(it->second);
	if( ptr )
	{
		if( ptr->is_valid() )
			ptr->expand();
		return ptr;
	}
	return sesn_ptr();
}

template <class Sesn>
std::shared_ptr<Sesn> request::session(bool create)
{
	static_assert(gts_is_base_of(http::session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");

	using sesn_ptr = std::shared_ptr<Sesn>;
	auto it = cookies().find("session_id");

	if( it == cookies().end() )
	{
		if( not create )
			return sesn_ptr();

		auto ptr = http::make_session<Sesn>();
		set_cookie_session_id(ptr->id());
		return ptr;
	}
	auto ptr = http::session::get<Sesn>(it->second);
	if( ptr )
	{
		if( ptr->is_valid() )
			ptr->expand();
		return ptr;
	}
	else if( create )
	{
		auto ptr = http::make_session<Sesn>();
		set_cookie_session_id(ptr->id());
		return ptr;
	}
	return sesn_ptr();
}

template <typename T>
T request::parameter(const std::string &key) const noexcept(false) {
	return parameter(key).get<T>();
}

template <typename T, typename U>
T request::parameter_or(const std::string &key, T deft_value) const noexcept {
	return parameter_or(key, http::value(std::move(deft_value))).get<T>();
}

template <typename T>
T request::header(const std::string &key) const noexcept(false) {
	return header(key).get<T>();
}

template <typename T, typename U>
T request::header_or(const std::string &key, T deft_value) const noexcept {
	return header_or(key, http::value(std::move(deft_value))).get<T>();
}

template <typename T>
T request::cookie(const std::string &key) const noexcept(false) {
	return cookie(key).get<T>();
}

template <typename T, typename U>
T request::cookie_or(const std::string &key, T deft_value) const noexcept {
	return cookie_or(key, http::value(std::move(deft_value))).get<T>();
}

inline std::string request::read_body(std::size_t size)
{
	std::error_code error;
	return read_body(error, size);
}

inline std::size_t request::read_body(void *buf, std::size_t size)
{
	std::error_code error;
	return read_body(error, buf, size);
}

inline std::string request::read_all_body()
{
	std::error_code error;
	return read_all_body(error);
}

inline bool request::save_file(const std::string &file_name, std::size_t begin)
{
	std::error_code error;
	return save_file(file_name, error, begin);
}

}} //namespace gts::http


#endif //GTS_HTTP_DETAIL_REQUEST_H
