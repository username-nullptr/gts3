#ifndef GTS_HTTP_COOKIES_H
#define GTS_HTTP_COOKIES_H

#include <gts/http/container.h>

namespace gts { namespace http
{

using cookie_attribute  = http::pair<http::value>;
using cookie_attributes = http::map<http::value, less_case_insensitive>;

class GTSWEB_API cookie : public http::value
{
public:
	using _vbase = http::value;
	using _vbase::value;

public:
	cookie(cookie &&other);
	cookie &operator=(cookie &&other);
	cookie(const cookie &other) = default;
	cookie &operator=(const cookie &other) = default;

public:
	std::string domain() const;
	std::string path() const;
	std::size_t size() const;

	uint64_t expires() const;
	uint64_t max_age() const;

	bool http_only() const;
	bool secure() const;

	std::string same_site() const;
	std::string priority() const;

public:
	cookie &set_domain(const std::string &domain);
	cookie &set_path(const std::string &path);
	cookie &set_size(std::size_t size);

	cookie &set_expires(uint64_t seconds);
	cookie &set_max_age(uint64_t seconds);

	cookie &set_http_only(bool flag);
	cookie &set_secure(bool flag);

	cookie &same_site(const std::string &sst);
	cookie &priority(const std::string &pt);

public:
	const cookie_attributes &attributes() const;

	cookie &set_attribute(const std::string &key, const std::string &value);
	cookie &set_attribute(const std::string &key, std::string &&value);

	template <typename...Args>
	cookie &set_attribute(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T, typename U = value::not_string_t<T,int>>
	cookie &set_attribute(const std::string &key, T &&value);

	cookie &unset_attribute(const std::string &key);

public:
	cookie &set_value(const std::string &v)
	{
		value::operator=(v);
		return *this;
	}

	cookie &set_value(std::string &&v)
	{
		value::operator=(std::move(v));
		return *this;
	}

	template <typename...Args>
	cookie &set_value(fmt::format_string<Args...> fmt_value, Args&&...args)
	{
		value::operator=(fmt::format(fmt_value, std::forward<Args>(args)...));
		return *this;
	}

	template <typename T, typename U = not_string_t<T,int>>
	cookie &set_value(T &&v)
	{
		value::set_value(std::forward<T>(v));
		return *this;
	}

public:
	template <typename...Args>
	static cookie from(fmt::format_string<Args...> fmt_value, Args&&...args)
	{
		cookie c;
		c.set_value(fmt_value, std::forward<Args>(args)...);
		return c;
	}

	template <typename T, typename U = not_string_t<T,int>>
	static cookie from(T &&v)
	{
		cookie c;
		set_value(std::forward<T>(v));
		return c;
	}

private:
	cookie_attributes m_attributes;
};

using basic_cookie_pair = http::pair<value>;
using basic_cookies     = http::unordered_map<value>;

using cookie_pair  = http::pair<cookie>;
using cookies      = http::unordered_map<cookie>;

template <typename...Args>
cookie &cookie::set_attribute(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	m_attributes[key].set_value(fmt_value, std::forward<Args>(args)...);
	return *this;
}

template <typename T, typename U>
cookie &cookie::set_attribute(const std::string &key, T &&value)
{
	m_attributes[key].set_value(std::forward<T>(value));
	return *this;
}

}} //namespace gts::http


#endif //GTS_HTTP_COOKIES_H
