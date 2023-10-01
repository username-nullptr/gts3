
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef GTS_HTTP_COOKIES_H
#define GTS_HTTP_COOKIES_H

#include <gts/http/container.h>

namespace gts { namespace http
{

using cookie_attribute  = http::pair<http::value>;
using cookie_attributes = http::map<http::value, less_case_insensitive>;

class GTS_HTTP_API cookie : public http::value
{
	template <typename CT>
	using not_cookie_t = enable_if_t<not gts_is_dsame(CT, cookie) and not is_string<CT>::value, int>;

public:
	using _vbase = http::value;
	using _vbase::value;

public:
	cookie();
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
	cookie &set_domain(std::string domain);
	cookie &set_path(std::string path);
	cookie &set_size(std::size_t size);

	cookie &set_expires(uint64_t seconds);
	cookie &set_max_age(uint64_t seconds);

	cookie &set_http_only(bool flag);
	cookie &set_secure(bool flag);

	cookie &same_site(std::string sst);
	cookie &priority(std::string pt);

public:
	cookie_attributes &attributes();
	const cookie_attributes &attributes() const;

	cookie &set_attribute(std::string key, std::string value);

	template <typename...Args>
	cookie &set_attribute(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T, typename U = not_cookie_t<T>>
	cookie &set_attribute(std::string key, T &&value);

	cookie &unset_attribute(const std::string &key);

public:
	cookie &set_value(std::string v);

	template <typename...Args>
	cookie &set_value(fmt::format_string<Args...> fmt_value, Args&&...args);

	template <typename T, typename U = not_cookie_t<T>>
	cookie &set_value(T &&v);

public:
	template <typename...Args>
	static cookie from(fmt::format_string<Args...> fmt_value, Args&&...args);

	template <typename T, typename U = not_cookie_t<T>>
	static cookie from(T &&v);

private:
	cookie_attributes m_attributes;
};

using basic_cookie_pair = http::pair<value>;
using basic_cookies = http::unordered_map<value>;

using cookie_pair = http::pair<cookie>;
using cookies = http::unordered_map<cookie>;

}} //namespace gts::http

#include <gts/http/detail/cookies.h>

#endif //GTS_HTTP_COOKIES_H
