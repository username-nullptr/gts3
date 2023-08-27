#ifndef GTS_HTTP_RESPONSE_H
#define GTS_HTTP_RESPONSE_H

#include <gts/http/types.h>
#include <gts/websocket.h>
#include <functional>

namespace gts { namespace http
{

class request;
class response_impl;
class file_transfer;

class GTSWEB_API response
{
	GTS_DISABLE_COPY_MOVE(response)

	template <typename T>
	using is_string = http::value::is_string<T>;

	template <typename CT>
	using not_value_t = enable_if_t<not is_string<CT>::value, int>;

	template <typename CT>
	using not_cookie_t = enable_if_t<not gts_is_dsame(CT, http::cookie) and not is_string<CT>::value, int>;

public:
	explicit response(http::request &request, http::status status = hs_ok);
	response(http::request &request, const http::headers &headers, http::status status = hs_ok);
	~response();

public:
	response &set_status(int status);
	response &set_headers(const http::headers &headers);
	response &set_cookies(const http::cookies &cookies);

public:
	response &set_header(std::string key, http::value value);

	template <typename...Args>
	response &set_header(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

public:
	response &set_cookie(std::string key, http::cookie cookie);

	template <typename...Args>
	response &set_cookie(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

public:
	std::string version() const;
	http::status status() const;

public:
	const http::headers &headers() const;
	http::headers &headers();
	const http::cookies &cookies() const;
	http::cookies &cookies();

public:
	bool headers_contains(const std::string &key) const;
	value &header(const std::string &key);
	const value &header(const std::string &key) const;
	value header_or(const std::string &key, value deft_value = {}) const;

	template <typename T>
	T header(const std::string &key) const;

	template <typename T, typename U = not_value_t<T>>
	T header_or(const std::string &key, T deft_value) const;

public:
	bool cookies_contains(const std::string &key) const;
	http::cookie &cookie(const std::string &key);
	const http::cookie &cookie(const std::string &key) const;
	http::cookie cookie_or(const std::string &key, http::cookie deft_value = {}) const;

	template <typename T>
	T cookie(const std::string &key) const;

	template <typename T, typename U = not_cookie_t<T>>
	T cookie_or(const std::string &key, T deft_value) const;

public:
	response &write_default();
	response &write_default(int status);

public:
	response &write();
	response &write(std::size_t size, const void *body);
	response &write(const std::string &body);
	response &write(const char *body);

	template <typename...Args>
	response &write(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T, typename U = not_cookie_t<T>>
	response &write(T &&value);

public:
	response &set_chunk_attribute(http::value attribute);
	response &set_chunk_attributes(value_list attributes);

	template <typename...Args>
	response &set_chunk_attribute(fmt::format_string<Args...> fmt, Args&&...args);
	response &chunk_end(const http::headers &headers = {});

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
	static void set_default_write(std::function<void(response&)>);
	bool is_writed() const;

public:
	websocket_ptr to_websocket() const; //TODO
	tcp_socket_ptr take() const;
	bool is_valid() const;

private:
	friend class file_transfer;
	response_impl *m_impl;
};

template <typename...Args>
response &response::set_header(std::string key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_header(std::move(key), fmt::format(value_fmt, std::forward<Args>(args)...));
}

template <typename...Args>
response &response::set_cookie(std::string key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_cookie(std::move(key), http::cookie::from(value_fmt, std::forward<Args>(args)...));
}

template <typename T>
T response::header(const std::string &key) const {
	return header(key).get<T>();
}

template <typename T, typename U>
T response::header_or(const std::string &key, T deft_value) const {
	return header_or(key, http::value(std::move(deft_value))).get<T>();
}

template <typename T>
T response::cookie(const std::string &key) const {
	return cookie(key).get<T>();
}

template <typename T, typename U>
T response::cookie_or(const std::string &key, T deft_value) const {
	return cookie_or(key, http::cookie(std::move(deft_value))).get<T>();
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

template <typename...Args>
response &response::write(fmt::format_string<Args...> value_fmt, Args&&...args)
{
	auto body = fmt::format(value_fmt, std::forward<Args>(args)...);
	return write(body.size(), body.c_str());
}

template <typename T, typename U>
response &response::write(T &&value) {
	return write("{}", value);
}

template <typename...Args>
response &response::set_chunk_attribute(fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_chunk_attribute(fmt::format(value_fmt, std::forward<Args>(args)...));
}

inline response &response::write_file(const std::string &file_name, const range &_range) {
	return write_file(file_name, range_vector{_range});
}

inline response &response::write_file(const std::string &file_name, std::size_t begin, std::size_t end) {
	return write_file(file_name, range_vector{{begin, end}});
}

}} //namespace gts::http


#endif //GTS_HTTP_RESPONSE_H
