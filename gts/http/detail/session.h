
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

#ifndef GTS_HTTP_DETAIL_SESSION_H
#define GTS_HTTP_DETAIL_SESSION_H

GTS_HTTP_NAMESPACE_BEGIN

template <class Sesn>
std::shared_ptr<Sesn> make_session(const std::chrono::seconds &seconds)
{
	static_assert(gts_is_base_of(session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");

	auto obj = new Sesn(seconds); obj->set(obj);
	return std::dynamic_pointer_cast<Sesn>(session::get(obj->id()));
}

template <typename T>
T session::attribute(const std::string &key) const noexcept(false) {
	return attribute(key).convert<T>();
}

template <typename T, typename U>
T session::attribute_or(const std::string &key, T deft_value) const noexcept {
	return attribute_or(key, rttr::variant(std::move(deft_value))).convert<T>();
}

template <typename...Args>
session &session::set_attribute(std::string key, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return set_attribute(std::move(key), fmt::format(fmt_value, std::forward<Args>(args)...));
}

template <class Sesn>
std::shared_ptr<Sesn> session::get(const std::string &id) noexcept(false)
{
	static_assert(gts_is_base_of(http::session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");

	auto ptr = get(id);
	if( ptr == nullptr )
		return std::shared_ptr<Sesn>();

	auto dy_ptr = std::dynamic_pointer_cast<Sesn>(ptr);
	if( dy_ptr == nullptr )
		throw exception("gts::http::session::get<{}>: The type of 'session' is incorrect.", type_name<Sesn>());
	return dy_ptr;
}

GTS_HTTP_NAMESPACE_END


#endif //GTS_HTTP_DETAIL_SESSION_H
