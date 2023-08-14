#ifndef GTS_HTTP_REQUEST_H
#define GTS_HTTP_REQUEST_H

#include <gts/http/types.h>
#include <asio.hpp>

namespace gts { namespace http
{

class request_impl;
class response_impl;

class GTSWEB_API request
{
	GTS_DISABLE_COPY_MOVE(request)

	template <typename T>
	using is_string = http::value::is_string<T>;

	template <typename CT>
	using not_value_t = enable_if_t<not is_string<CT>::value, int>;

public:
	request();
	~request();

public:
	http::method method() const;
	std::string version() const;
	std::string path() const;

public:
	std::string parameters_string() const;
	const http::parameters &parameters() const;
	const http::headers &headers() const;
	const basic_cookies &cookies() const;

public:
	template <class Sesn = http::session>
	std::shared_ptr<Sesn> session() const;

	template <class Sesn = http::session>
	std::shared_ptr<Sesn> session(bool create = true);

public:
	bool parameters_contains(const std::string &key) const;
	const value &parameter(const std::string &key) const;
	value parameter_or(const std::string &key, value deft_value = {}) const;

	template <typename T>
	T parameter(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T parameter_or(const std::string &key, T deft_value) const;

public:
	bool headers_contains(const std::string &key) const;
	const value &header(const std::string &key) const;
	value header_or(const std::string &key, value deft_value = {}) const;

	template <typename T>
	T header(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T header_or(const std::string &key, T deft_value) const;

public:
	bool cookies_contains(const std::string &key) const;
	const value &cookie(const std::string &key) const;
	value cookie_or(const std::string &key, value deft_value) const;

	template <typename T>
	T cookie(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T cookie_or(const std::string &key, T deft_value) const;

public:
	std::string read_body(std::error_code &error, std::size_t size = 0);
	std::size_t read_body(std::error_code &error, void *buf, std::size_t size);
	std::string read_body(std::size_t size = 0);
	std::size_t read_body(void *buf, std::size_t size);

public:
    std::string read_all_body(std::error_code &error);
    std::string read_all_body();

public:
	bool save_file(const std::string &file_name, std::error_code &error, std::size_t begin = 0);
	bool save_file(const std::string &file_name, std::size_t begin = 0);

public:
	bool keep_alive() const;
	bool support_gzip() const;
	bool can_read_body() const;
	bool is_valid() const;

public:
	asio::ip::tcp::endpoint remote_endpoint() const;
	asio::ip::tcp::endpoint local_endpoint() const;

private:
	void set_cookie_session_id(std::string id);

private:
	friend class request_impl;
	friend class response_impl;
	friend class web::http_parser;
	friend class web::service_io;
	friend class web::session;
	request_impl *m_impl;
};

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
T request::parameter(const std::string &key) const {
	return parameter(key).get<T>();
}

template <typename T, typename U>
T request::parameter_or(const std::string &key, T deft_value) const {
	return parameter_or(key, http::value(std::move(deft_value))).get<T>();
}

template <typename T>
T request::header(const std::string &key) const {
	return header(key).get<T>();
}

template <typename T, typename U>
T request::header_or(const std::string &key, T deft_value) const {
	return header_or(key, http::value(std::move(deft_value))).get<T>();
}

template <typename T>
T request::cookie(const std::string &key) const {
	return cookie(key).get<T>();
}

template <typename T, typename U>
T request::cookie_or(const std::string &key, T deft_value) const {
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


#endif //GTS_HTTP_REQUEST_H
