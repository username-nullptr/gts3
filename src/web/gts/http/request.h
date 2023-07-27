﻿#ifndef GTS_HTTP_REQUEST_H
#define GTS_HTTP_REQUEST_H

#include <gts/http/types.h>

namespace gts {
class tcp_socket;

namespace http
{

class request_impl;

class GTSWEB_API request
{
	GTS_DISABLE_COPY(request)

	template <typename T>
	using not_value_t = enable_if_t<not gts_is_dsame(T, http::value), int>;

public:
	request();
	request(request &&other);
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
	session_ptr session() const;
	session_ptr session(bool create = true);

	template <class Sesn>
	std::shared_ptr<Sesn> session() const;

	template <class Sesn>
	std::shared_ptr<Sesn> session(bool create = true);

public:
	const value &parameter(const std::string &key) const;
	value parameter_or(const std::string &key, const value &deft_value) const;
	value parameter_or(const std::string &key, value &&deft_value = {}) const;

	template <typename T>
	T parameter(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T parameter_or(const std::string &key, const T &deft_value) const;

public:
	const value &header(const std::string &key) const;
	value header_or(const std::string &key, const value &deft_value) const;
	value header_or(const std::string &key, value &&deft_value = {}) const;

	template <typename T>
	T header(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T header_or(const std::string &key, const T &deft_value) const;

public:
	const value &cookie(const std::string &key) const;
	value cookie_or(const std::string &key, const value &deft_value) const;
	value cookie_or(const std::string &key, value &&deft_value = {}) const;

	template <typename T>
	T cookie(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T cookie_or(const std::string &key, const T &deft_value) const;

public:
	std::string read_body(std::error_code &error, std::size_t size = 0);
	std::size_t read_body(std::error_code &error, void *buf, std::size_t size);
	std::string read_body(std::size_t size = 0);
	std::size_t read_body(void *buf, std::size_t size);

public:
	bool save_file(const std::string &file_name, std::error_code &error);
	bool save_file(const std::string &file_name);

public:
	bool keep_alive() const;
	bool support_gzip() const;

public:
	request &operator=(request &&other);
	bool is_valid() const;

public:
	const tcp_socket &socket() const;
	tcp_socket &socket();

private:
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
	std::dynamic_pointer_cast<Sesn>(session());
}

template <class Sesn>
std::shared_ptr<Sesn> request::session(bool create)
{
	static_assert(gts_is_base_of(http::session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");
	std::dynamic_pointer_cast<Sesn>(session(create));
}

template <typename T>
T request::parameter(const std::string &key) const {
	return parameter(key).get<T>();
}

template <typename T, typename U>
T request::parameter_or(const std::string &key, const T &deft_value) const {
	return parameter_or(key, http::value(deft_value)).get<T>();
}

template <typename T>
T request::header(const std::string &key) const {
	return header(key).get<T>();
}

template <typename T, typename U>
T request::header_or(const std::string &key, const T &deft_value) const {
	return header_or(key, http::value(deft_value)).get<T>();
}

template <typename T>
T request::cookie(const std::string &key) const {
	return cookie(key).get<T>();
}

template <typename T, typename U>
T request::cookie_or(const std::string &key, const T &deft_value) const {
	return cookie_or(key, http::value(deft_value)).get<T>();
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

inline bool request::save_file(const std::string &file_name)
{
	std::error_code error;
	return save_file(file_name, error);
}

}} //namespace gts::http


#endif //GTS_HTTP_REQUEST_H
