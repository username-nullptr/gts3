
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

#ifndef GTS_HTTP_SESSION_H
#define GTS_HTTP_SESSION_H

#include <gts/http/container.h>
#include <gts/exception.h>

GTS_HTTP_NAMESPACE_BEGIN

class session_impl;

using session_attribute  = std::pair<std::string, rttr::variant>;
using session_attributes = std::unordered_map<std::string, rttr::variant>;

class GTS_HTTP_API session
{
	GTS_DISABLE_COPY_MOVE(session)
	using duration = std::chrono::seconds;

	template <typename T>
	using not_variant_t = enable_if_t<not gts_is_dsame(T, rttr::variant), int>;

public:
	explicit session(const duration &seconds = duration(0));
	virtual ~session();

public:
	std::string id() const;
	uint64_t create_time() const;
	bool is_valid() const;

public:
	rttr::variant attribute(const std::string &key) const noexcept(false);
	rttr::variant attribute_or(const std::string &key, rttr::variant deft_value = {}) const noexcept;

	template <typename T>
	T attribute(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_variant_t<T>>
	T attribute_or(const std::string &key, T deft_value) const noexcept;

public:
	session &set_attribute(std::string key, rttr::variant value);

	template <typename...Args>
	session &set_attribute(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

	session &unset_attribute(const std::string &key);

public:
	std::size_t attribute_count() const;
	session_attributes attributes() const;
	string_list attribute_key_list() const;
	std::set<std::string> attribute_key_set() const;

public:
	uint64_t lifecycle() const;
	session &set_lifecycle(const duration &seconds = duration(0));
	session &expand(const duration &seconds = duration(0));
	void invalidate();

public:
	static std::shared_ptr<session> make_shared(const duration &seconds = duration(0));
	static std::shared_ptr<session> get(const std::string &id);
	static void set(session *obj);

	template <class Sesn>
	static std::shared_ptr<Sesn> get(const std::string &id) noexcept(false);

public:
	static void set_global_lifecycle(const duration &seconds);
	static duration global_lifecycle();

private:
	session_impl *m_impl;
};

using session_ptr = std::shared_ptr<session>;

template <class Sesn = session>
std::shared_ptr<Sesn> make_session(const std::chrono::seconds &seconds = std::chrono::seconds(0));

GTS_HTTP_NAMESPACE_END
#include <gts/http/detail/session.h>

#endif //GTS_HTTP_SESSION_H
