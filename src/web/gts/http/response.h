#ifndef GTS_HTTP_RESPONSE_H
#define GTS_HTTP_RESPONSE_H

#include <gts/http/type.h>
#include <fmt/format.h>

namespace gts {
class tcp_socket;

namespace http
{

#define _GTS_HTTP_RESPONSE_NOT_STRING \
	template <typename T> \
	enable_if_t< \
		not gts_is_same(decay_t<T>, std::string) and \
		not gts_is_same(decay_t<T>, char*), \
	response>&

class request;
class response_impl;

class GTSWEB_API response
{
	GTS_DISABLE_COPY(response)

public:
	explicit response(http::request &request, http::status status = hs_ok);
	response(http::request &request, const http::headers &headers, http::status status = hs_ok);

public:
	response(response &&other);
	~response();

public:
	response &set_status(http::status status);
	response &set_header(const std::string &key, const std::string &value);
	response &set_headers(const http::headers &headers);

public:
	template <typename...Args>
	response &set_header(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);
	_GTS_HTTP_RESPONSE_NOT_STRING set_header(const std::string &key, T &&value);

public:
	std::string version() const;
	const http::headers &headers() const;
	http::status status() const;

public:
	response &write_default();
	response &write_default(http::status status);

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
	_GTS_HTTP_RESPONSE_NOT_STRING write(T &&value);
	_GTS_HTTP_RESPONSE_NOT_STRING write_body(T &&value);

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
	void close(bool force = false);

public:
	const tcp_socket &socket() const;
	tcp_socket &socket();

public:
	response &operator=(response &&other);

private:
	response_impl *m_impl;
};

inline response &response::set_headers(const http::headers &headers)
{
	for(auto &p : headers)
		set_header(p.first, p.second);
	return *this;
}

template <typename...Args> inline
response &response::set_header(const std::string &key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_header(key, fmt::format(value_fmt, std::forward<Args>(args)...));
}

_GTS_HTTP_RESPONSE_NOT_STRING response::set_header(const std::string &key, T &&value) {
	return set_header(key, fmt::format("{}", std::forward<T>(value)));
}

inline response &response::write_default(http::status status) {
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

_GTS_HTTP_RESPONSE_NOT_STRING response::write(T &&value) {
	return write("{}", value);
}

_GTS_HTTP_RESPONSE_NOT_STRING response::write_body(T &&value) {
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
