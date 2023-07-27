#ifndef GTS_HTTP_RESPONSE_H
#define GTS_HTTP_RESPONSE_H

#include <gts/http/types.h>
#include <functional>

namespace gts {
class tcp_socket;

namespace http
{

class request;
class response_impl;

class GTSWEB_API response
{
	GTS_DISABLE_COPY(response)

	template <typename T>
	using is_string = http::value::is_string<T>;

	template <typename CT, typename V>
	using not_type_t = enable_if_t<not gts_is_dsame(CT, V) and not is_string<CT>::value, int>;

	template <typename T>
	using not_value_t = enable_if_t<not gts_is_dsame(T, http::value), int>;

public:
	explicit response(http::request &request, http::status status = hs_ok);
	response(http::request &request, const http::headers &headers, http::status status = hs_ok);

public:
	response(response &&other);
	~response();

public:
	response &set_status(int status);
	response &set_headers(const http::headers &headers);
	response &set_cookies(const http::cookies &cookies);

public:
	response &set_header(const std::string &key, const std::string &value);
	response &set_header(const std::string &key, std::string &&value);

	template <typename...Args>
	response &set_header(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T, typename U = not_type_t<T,http::value>>
	response &set_header(const std::string &key, T &&value);

public:
	response &set_cookie(const std::string &key, const http::cookie &cookie);
	response &set_cookie(const std::string &key, http::cookie &&cookie);

	template <typename...Args>
	response &set_cookie(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T, typename U = not_type_t<T,http::cookie>>
	response &set_cookie(const std::string &key, T &&value);

public:
	std::string version() const;
	http::status status() const;

public:
	const http::headers &headers() const;
	http::headers &headers();
	const http::cookies &cookies() const;
	http::cookies &cookies();

public:
	value &header(const std::string &key);
	const value &header(const std::string &key) const;
	value header_or(const std::string &key, const value &deft_value) const;
	value header_or(const std::string &key, value &&deft_value = {}) const;

	template <typename T>
	T header(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T header_or(const std::string &key, const T &deft_value) const;

public:
	http::cookie &cookie(const std::string &key);
	const http::cookie &cookie(const std::string &key) const;
	http::cookie cookie_or(const std::string &key, const http::cookie &deft_value) const;
	http::cookie cookie_or(const std::string &key, http::cookie &&deft_value = {}) const;

	template <typename T>
	T cookie(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T cookie_or(const std::string &key, const T &deft_value) const;

public:
	response &write_default();
	response &write_default(int status);

public:
	response &write();
	response &write(std::size_t size, const void *body);
	response &write(const std::string &body);
	response &write(const char *body);

public:
	response &write_body(std::size_t size, const void *body);
	response &write_body(const std::string &body);
	response &write_body(const char *body);

public:
	template <typename...Args>
	response &write(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename...Args>
	response &write_body(fmt::format_string<Args...> fmt, Args&&...args);

public:
	template <typename T, typename U = value::not_type_t<T,int>>
	response &write(T &&value);

	template <typename T, typename U = value::not_type_t<T,int>>
	response &write_body(T &&value);

public:
	response &write_file(const std::string &file_name);
	response &write_file(const std::string &file_name, const std::string &range_http_value);
	response &write_file(const std::string &file_name, std::size_t begin, std::size_t end);

public:
	struct range
	{
		std::size_t begin;
		std::size_t end;
	};
	using range_vector = std::vector<range>;
	response &write_file(const std::string &file_name, const response::range &range);
	response &write_file(const std::string &file_name, const range_vector &range);

public:
	response &redirect(const std::string &url, redirect_type type = redirect_type::moved_permanently);

public:
	response &unset_header(const std::string &key);
	response &unset_cookie(const std::string &key);
	void close(bool force = false);

public:
	const tcp_socket &socket() const;
	tcp_socket &socket();

public:
	response &operator=(response &&other);
	static void set_default_write(std::function<void(response&)>);

private:
	response_impl *m_impl;
};

inline response &response::set_headers(const http::headers &headers)
{
	for(auto &p : headers)
		set_header(p.first, p.second);
	return *this;
}

inline response &response::set_cookies(const http::cookies &cookies)
{
	for(auto &p : cookies)
		set_cookie(p.first, p.second);
	return *this;
}

template <typename...Args>
response &response::set_header(const std::string &key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_header(key, fmt::format(value_fmt, std::forward<Args>(args)...));
}

template <typename T, typename U>
response &response::set_header(const std::string &key, T &&value) {
	return set_header(key, "{}", std::forward<T>(value));
}

template <typename...Args>
response &response::set_cookie(const std::string &key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_cookie(key, http::cookie::from(value_fmt, std::forward<Args>(args)...));
}

template <typename T, typename U>
response &response::set_cookie(const std::string &key, T &&value) {
	return set_cookie(key, http::cookie(std::forward<T>(value)));
}

template <typename T>
T response::header(const std::string &key) const {
	return header(key).get<T>();
}

template <typename T, typename U>
T response::header_or(const std::string &key, const T &deft_value) const {
	return header_or(key, http::value(deft_value)).get<T>();
}

template <typename T>
T response::cookie(const std::string &key) const {
	return cookie(key).get<T>();
}

template <typename T, typename U>
T response::cookie_or(const std::string &key, const T &deft_value) const {
	return cookie_or(key, http::cookie(deft_value)).get<T>();
}

inline response &response::write_default(int status) {
	return set_status(status).write_default();
}

inline response &response::write() {
	return write(0,nullptr);
}

inline response &response::write(const std::string &body) {
	return write(body.size(), body.c_str());
}

inline response &response::write(const char *body) {
	return write(strlen(body), body);
}

inline response &response::write_body(const std::string &body) {
	return write_body(body.size(), body.c_str());
}

inline response &response::write_body(const char *body) {
	return write_body(strlen(body), body);
}

template <typename...Args>
response &response::write(fmt::format_string<Args...> value_fmt, Args&&...args)
{
	auto body = fmt::format(value_fmt, std::forward<Args>(args)...);
	return write(body.size(), body.c_str());
}

template <typename...Args>
response &response::write_body(fmt::format_string<Args...> value_fmt, Args&&...args)
{
	auto body = fmt::format(value_fmt, std::forward<Args>(args)...);
	return write_body(body.size(), body.c_str());
}

template <typename T, typename U>
response &response::write(T &&value) {
	return write("{}", value);
}

template <typename T, typename U>
response &response::write_body(T &&value) {
	return write_body("{}", value);
}

inline response &response::write_file(const std::string &file_name, const range &_range) {
	return write_file(file_name, range_vector{_range});
}

inline response &response::write_file(const std::string &file_name, std::size_t begin, std::size_t end) {
	return write_file(file_name, range_vector{{begin, end}});
}

}} //namespace gts::http


#endif //GTS_HTTP_RESPONSE_H
