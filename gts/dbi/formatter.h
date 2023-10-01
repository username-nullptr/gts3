
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

#ifndef GTS_DBI_FORMATTER_H
#define GTS_DBI_FORMATTER_H

#include <gts/dbi/types.h>
#include <cppformat>

namespace fmt
{

template <>
class formatter<gts::dbi::error_code>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const gts::dbi::error_code &error, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", error.message(), error.value());
	}
};

template <>
class formatter<gts::dbi::cell>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const gts::dbi::cell &data, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", data.has_value()? data.to_string() : "NULL");
	}
};

template <>
class formatter<gts::dbi::table_data>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const gts::dbi::table_data &table, Context &&context) -> decltype(context.out())
	{
		std::string str;
		for(auto &rows : table)
		{
			str += "{";
			for(auto &cell : rows)
				str += "'" + (cell.has_value()? cell.to_string() : "NULL") + "'";
			str += "}";
		}
		return format_to(context.out(), "\"{}\"", str);
	}
};

} //namespace fmt


#endif //GTS_DBI_FORMATTER_H
