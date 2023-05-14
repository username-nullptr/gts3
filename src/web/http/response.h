#ifndef RESPONSE_H
#define RESPONSE_H

#include "protocol.h"
#include <fmt/format.h>

namespace gts { namespace web { namespace http
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

class GTS_DECL_HIDDEN response
{
	DISABLE_COPY(response)

public:
	explicit response(status s = hs_ok);
	explicit response(const headers &h, status s = hs_ok);
	explicit response(const std::string &v, status s = hs_ok);
	explicit response(const std::string &v, const headers &h, status s = hs_ok);
	~response();

public:
	void set_status(status s);
	void set_header(const std::string &key, const std::string &value);
	void set_headers(const headers &h);

public:
	template <typename...Args>
	void set_header(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);
	_GTS_HTTP_RESPONSE_NOT_STRING set_header(const std::string &key, T &&value);

public:
	std::string to_string(bool end = false) const;

private:
	std::string m_version = "1.1";
	status m_status = hs_ok;
	headers m_headers;
};

inline void response::set_headers(const headers &h)
{
	for(auto &p : h)
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

}}} //namespace gts::web::http


#endif //RESPONSE_H
