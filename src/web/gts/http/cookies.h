#ifndef GTS_HTTP_COOKIES_H
#define GTS_HTTP_COOKIES_H

#include <gts/http/container.h>

namespace gts { namespace http
{

class cookie_value_impl;

class GTSWEB_API cookie_attribute : public value
{
public:
	enum same_site_type {
		strict, lax, none,
	};
	enum priority_type {
		low, medium, high
	};

public:
	using value::value;
	using value::operator+=;
	using value::operator=;
	using value::operator[];

public:
	std::string domain() const;
	std::string path()   const;
	std::size_t size()   const;

	uint64_t create_time() const;
	uint64_t expires() const;
	uint64_t max_age() const;

	bool http_only() const;
	bool secure() const;

	same_site_type same_site() const;
	priority_type priority() const;

public:
	void set_domain(const std::string &domain);
	void set_path(const std::string &path);
	void set_size(std::size_t size);

	void set_expires(uint64_t seconds) const;
	void set_max_age(uint64_t seconds) const;

	void set_http_only(bool flag);
	void set_secure(bool flag);

	void same_site(same_site_type sst);
	void priority(priority_type pt);

private:
	cookie_value_impl *m_impl;
};

using basic_cookie  = http::pair<value>;
using basic_cookies = http::unordered_map<value>;

using cookie  = http::pair<cookie_attribute>;
using cookies = http::unordered_map<cookie_attribute>;

}} //namespace gts::http


#endif //GTS_HTTP_COOKIES_H
