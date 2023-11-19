
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

#ifndef GTS_DBI_EXCEPTION_H
#define GTS_DBI_EXCEPTION_H

#include <gts/dbi/global.h>
#include <gts/exception.h>

GTS_DBI_NAMESPACE_BEGIN

class error_code
{
public:
	error_code() = default;
	error_code(int value, std::string message);
	error_code(const error_code &other);
	error_code(error_code &&other) noexcept;

public:
	int value() const;
	std::string message() const;

public:
	operator bool() const;

public:
	error_code &operator=(const error_code &other);
	error_code &operator=(error_code &&other) noexcept;

protected:
	int m_value = 0;
	std::string m_message = "success";
};

class exception : public gts::basic_exception, public error_code
{
public:
	exception(int value, std::string message);
	exception(const error_code &other);
	exception(error_code &&other) noexcept;

public:
	template <typename...Args>
	explicit exception(int value, fmt::format_string<Args...> fmt_value, Args&&...args);

public:
	const char *what() const _GLIBCXX_USE_NOEXCEPT override;
#if GTS_CPLUSPLUS >= 201703L
private: GTS_DISABLE_COPY_MOVE(exception)
#else
public:
	exception(const exception&) = default;
	exception(exception &&other) noexcept;
#endif
};

GTS_DBI_NAMESPACE_END

namespace fmt
{

template <>
class formatter<gts::dbi::exception> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const gts::dbi::exception &ex, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", ex.what(), ex.value());
	}
};

} //namespace fmt

#include <gts/dbi/detail/error.h>

#endif //GTS_DBI_EXCEPTION_H
