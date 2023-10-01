
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

#ifndef GTS_EXCEPTION_H
#define GTS_EXCEPTION_H

#include <gts/global.h>
#include <gts/formatter.h>
#include <exception>

namespace gts
{

class basic_exception : public std::exception {};

class exception : public basic_exception
{
public:
	explicit exception(std::string what);

	template <typename...Args>
	explicit exception(fmt::format_string<Args...> fmt_value, Args&&...args);

public:
	const char* what() const _GLIBCXX_NOTHROW override;
#if GTS_CPLUSPLUS >= 201703L
private: GTS_DISABLE_COPY_MOVE(exception)
#else
public:
	exception(const exception&) = default;
	exception(exception &&other) noexcept;
#endif
private:
	std::string m_what;
};

} //namespace gts

namespace fmt
{

template <>
class formatter<gts::exception> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const gts::exception &ex, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", ex.what());
	}
};

} //namespace fmt

#include <gts/detail/exception.h>

#endif //GTS_EXCEPTION_H
