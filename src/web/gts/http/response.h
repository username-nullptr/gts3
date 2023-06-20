#ifndef GTS_HTTP_RESPONSE_H
#define GTS_HTTP_RESPONSE_H

#include <gts/http/global.h>
#include <fmt/format.h>

namespace gts { namespace http
{

#if __cplusplus < 201703L
# define _GTS_HTTP_RESPONSE_NOT_STRING \
	template <typename T> \
	typename std::enable_if< \
		not std::is_same<typename std::decay<T>::type, std::string>::value and \
		not std::is_same<typename std::decay<T>::type, char*>::value \
	>::type
#else //c++2017
# define _GTS_HTTP_RESPONSE_NOT_STRING \
	template <typename T> \
	std::enable_if_t< \
		not std::is_same_v<std::decay_t<T>, std::string> and \
		not std::is_same_v<std::decay_t<T>, char*> \
	>
#endif //c++2017

class response_private;

class GTSHTTP_API response
{
	GTS_DISABLE_COPY(response)

public:
	explicit response(http::status status = hs_ok);
	explicit response(const http::headers &headers, http::status status = hs_ok);
	explicit response(const std::string &v, http::status status = hs_ok);
	response(const std::string &v, const http::headers &headers, http::status status = hs_ok);

public:
	response(response &&other);
	~response();

public:
	void set_status(http::status status);
	void set_header(const std::string &key, const std::string &value);
	void set_headers(const http::headers &headers);

public:
	template <typename...Args>
	void set_header(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);
	_GTS_HTTP_RESPONSE_NOT_STRING set_header(const std::string &key, T &&value);

public:
	const std::string &version() const;
	const http::headers &headers() const;
	http::status status() const;

public:
	std::string to_string(bool end = true) const;
	response &operator=(response &&other);

private:
	response(const response&) = delete;
	void operator=(const response&) = delete;
	response_private *d_ptr;
};

inline void response::set_headers(const http::headers &headers)
{
	for(auto &p : headers)
		set_header(p.first, p.second);
}

template <typename...Args> inline
void response::set_header(const std::string &key, fmt::format_string<Args...> value_fmt, Args&&...args)
{
	set_header(key, fmt::format(value_fmt, std::forward<Args>(args)...));
}

_GTS_HTTP_RESPONSE_NOT_STRING response::set_header(const std::string &key, T &&value)
{
	set_header(key, fmt::format("{}", std::forward<T>(value)));
}

}} //namespace gts::http


#endif //GTS_HTTP_RESPONSE_H
