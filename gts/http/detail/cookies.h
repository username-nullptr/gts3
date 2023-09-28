#ifndef GTS_HTTP_DETAIL_COOKIES_H
#define GTS_HTTP_DETAIL_COOKIES_H

namespace gts { namespace http
{

template <typename...Args>
cookie &cookie::set_attribute(std::string key, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	m_attributes[std::move(key)].set_value(fmt_value, std::forward<Args>(args)...);
	return *this;
}

template <typename T, typename U>
cookie &cookie::set_attribute(std::string key, T &&value)
{
	m_attributes[std::move(key)].set_value(std::forward<T>(value));
	return *this;
}

inline cookie &cookie::set_value(std::string v)
{
	value::operator=(std::move(v));
	return *this;
}

template <typename...Args>
cookie &cookie::set_value(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	value::operator=(fmt::format(fmt_value, std::forward<Args>(args)...));
	return *this;
}

template <typename T, typename U>
cookie &cookie::set_value(T &&v)
{
	set_value("{}", std::forward<T>(v));
	return *this;
}

template <typename...Args>
cookie cookie::from(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	cookie c;
	c.set_value(fmt_value, std::forward<Args>(args)...);
	return c;
}

template <typename T, typename U>
cookie cookie::from(T &&v)
{
	cookie c;
	c.set_value(std::forward<T>(v));
	return c;
}

}} //namespace gts::http


#endif //GTS_HTTP_DETAIL_COOKIES_H
