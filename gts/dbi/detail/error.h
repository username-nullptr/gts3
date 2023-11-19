
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

#ifndef GTS_DBI_DETAIL_ERROR_H
#define GTS_DBI_DETAIL_ERROR_H

GTS_DBI_NAMESPACE_BEGIN

inline error_code::error_code(int value, std::string message) :
	m_value(value), m_message(std::move(message))
{

}

inline error_code::error_code(const error_code &other) :
	m_value(other.value()), m_message(other.m_message)
{

}

inline error_code::error_code(error_code &&other) noexcept :
	m_value(other.value()), m_message(std::move(other.m_message))
{

}

inline int error_code::value() const
{
	return m_value;
}

inline std::string error_code::message() const
{
	return m_message;
}

inline error_code::operator bool() const
{
	return value() != 0;
}

inline error_code &error_code::operator=(const error_code &other)
{
	m_value = other.m_value;
	m_message = other.m_message;
	return *this;
}

inline error_code &error_code::operator=(error_code &&other) noexcept
{
	m_value = other.value();
	m_message = std::move(other.m_message);
	return *this;
}

inline exception::exception(int value, std::string message) :
	error_code(value, std::move(message))
{

}

inline exception::exception(const error_code &other) :
	error_code(other)
{

}

inline exception::exception(error_code &&other) noexcept :
	error_code(std::move(other))
{

}

template <typename...Args>
exception::exception(int value, fmt::format_string<Args...> fmt_value, Args&&...args) :
	error_code(value, fmt::format(fmt_value, std::forward<Args>(args)...))
{

}

inline const char *exception::what() const _GLIBCXX_USE_NOEXCEPT
{
	return m_message.c_str();
}

#if GTS_CPLUSPLUS < 201703L

inline exception::exception(exception &&other) noexcept
{
	m_value = other.m_value;
	m_message = std::move(other.m_message);
}

#endif

GTS_DBI_NAMESPACE_END


#endif //GTS_DBI_DETAIL_ERROR_H
