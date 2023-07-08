#ifndef GTS_HTTP_COOKIES_H
#define GTS_HTTP_COOKIES_H

#include <gts/http/type.h>
#if 0

namespace gts { namespace http
{

class cookie_impl;

class GTSWEB_API cookie
{
public:
	enum same_site_type {
		strict, lax, none,
	};
	enum priority_type {
		low, medium, high
	};

public:
	std::string name()   const;
	std::string value()  const;
	std::string domain() const;
	std::string path()   const;
	std::size_t size()   const;

	bool http_only() const;
	bool secure() const;

	same_site_type same_site() const;
	priority_type priority() const;

public:
	void set_name(const std::string &name);
	void set_value(const std::string &value);
	void set_domain(const std::string &domain);
	void set_path(const std::string &path);
	void set_size(std::size_t size);

private:
	cookie_impl *m_impl;
};

}} //namespace gts::http

namespace std {
template<> struct hash<gts::http::cookie> : public __hash_base<size_t, gts::http::cookie> {
	size_t operator()(const gts::http::cookie& node) const noexcept {
		return hash<std::string>()(node.name());
	}
};
} //namespace std

namespace gts { namespace http {
using cookies = std::unordered_set<cookie>;
}} //namespace gts::http


#endif //0
#endif //GTS_HTTP_COOKIES_H
