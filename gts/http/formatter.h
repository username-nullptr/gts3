
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

#ifndef GTS_HTTP_FORMATTER_H
#define GTS_HTTP_FORMATTER_H

#include <gts/http/types.h>
#include <gts/formatter.h>

GTS_HTTP_NAMESPACE_BEGIN

class enum_formatter
{
protected:
	enum class mode {
		number, string, str_num
	}
	m_mode = mode::number;

public:
	template <typename Context>
	inline FMT_CONSTEXPR auto parse(Context &&context) noexcept(false) -> decltype(context.begin())
	{
		auto it = context.begin();
		if( it == context.end() or *it == '}' )
		{
			m_mode = mode::number;
			return it;
		}
		if( *it == 'n' )
		{
			m_mode = mode::number;
			++it;
		}
		else if( *it == 's' )
		{
			m_mode = mode::string;
			++it;
			if( *it == 'n' )
			{
				m_mode = mode::str_num;
				++it;
			}
		}
		if( it != context.end() and *it != '}' )
			fmt::detail::throw_format_error("Invalid gts::http enum format specifier.");
		return it;
	}
};

GTS_HTTP_NAMESPACE_END

namespace fmt
{

template <>
class formatter<gts::http::status> : public gts::http::enum_formatter
{
public:
	template <typename Context>
	inline auto format(gts::http::status status, Context &&context) -> decltype(context.out())
	{
		if( this->m_mode == mode::number )
			return format_to(context.out(), "{}", static_cast<int>(status));
		else if( this->m_mode == mode::string )
			return format_to(context.out(), "{}", gts::http::status_description(status));

		return format_to(context.out(), "{} ({})", gts::http::status_description(status), static_cast<int>(status));
	}
};

template <>
class formatter<gts::http::method> : public gts::http::enum_formatter
{
public:
	template <typename Context>
	inline auto format(gts::http::method method, Context &&context) -> decltype(context.out())
	{
		if( this->m_mode == mode::number )
			return format_to(context.out(), "{}", static_cast<int>(method));
		else if( this->m_mode == mode::string )
			return format_to(context.out(), "{}", gts::http::method_string(method));

		return format_to(context.out(), "{}({})", gts::http::method_string(method), static_cast<int>(method));
	}
};

template <>
class formatter<gts::http::cookie> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const gts::http::cookie &cookie, Context &&context) -> decltype(context.out())
	{
		std::string str = cookie + "; ";
		for(auto &attr_pair : cookie.attributes())
			str += attr_pair.first + "=" + attr_pair.second + "; ";
		str.pop_back();
		str.pop_back();
		return format_to(context.out(), "'{}'", str);
	}
};

template <>
class formatter<gts::http::cookie_pair> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const gts::http::cookie_pair &cookie_pair, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "'{}'-{}", cookie_pair.first, cookie_pair.second);
	}
};

} //namespace fmt


#endif //GTS_HTTP_FORMATTER_H
