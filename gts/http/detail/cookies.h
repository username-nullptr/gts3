
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

#ifndef GTS_HTTP_DETAIL_COOKIES_H
#define GTS_HTTP_DETAIL_COOKIES_H

GTS_HTTP_NAMESPACE_BEGIN

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

GTS_HTTP_NAMESPACE_END


#endif //GTS_HTTP_DETAIL_COOKIES_H
