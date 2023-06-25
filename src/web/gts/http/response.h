#ifndef GTS_HTTP_RESPONSE_H
#define GTS_HTTP_RESPONSE_H

#include <gts/tcp_socket.h>
#include <gts/http/type.h>
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

class response_impl;

class GTSWEB_API response
{
	GTS_DISABLE_COPY(response)

public:
	explicit response(tcp_socket_ptr socket, http::status status = hs_ok);
	response(tcp_socket_ptr socket, const http::headers &headers, http::status status = hs_ok);
	response(tcp_socket_ptr socket, const std::string &v, http::status status = hs_ok);
	response(tcp_socket_ptr socket, const std::string &v, const http::headers &headers, http::status status = hs_ok);

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
	void write(const void *body, std::size_t size, bool close = false);
	void write(const std::string &body = "", bool close = false);

public:
	void write_body(const void *body, std::size_t size, bool close = false);
	void write_body(const std::string &body, bool close = false);

public:
	template <typename...Args>
	void write(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename...Args>
	void write_body(fmt::format_string<Args...> fmt, Args&&...args);

public:
	void close(bool force = false);
	const tcp_socket &socket() const;
	tcp_socket &socket();

public:
	response &operator=(response &&other);

private:
	response_impl *m_impl;
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

inline void response::write(const void *body, std::size_t size, bool close)
{
	write(std::string(static_cast<const char*>(body), size), close);
}

inline void response::write_body(const void *body, std::size_t size, bool close)
{
	write_body(std::string(static_cast<const char*>(body), size), close);
}

template <typename...Args>
void response::write(fmt::format_string<Args...> value_fmt, Args&&...args)
{
	write(fmt::format(value_fmt, std::forward<Args>(args)...));
}

template <typename...Args>
void response::write_body(fmt::format_string<Args...> value_fmt, Args&&...args)
{
	write_body(fmt::format(value_fmt, std::forward<Args>(args)...));
}

}} //namespace gts::http


#endif //GTS_HTTP_RESPONSE_H
