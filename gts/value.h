
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

#ifndef GTS_VALUE_H
#define GTS_VALUE_H

#include <gts/algorithm.h>
#include <cppformat>

GTS_NAMESPACE_BEGIN

class value : public std::string
{
public:
	template <typename CT>
	struct is_string {
		static constexpr bool value = gts_is_same(decay_t<CT>, gts::value) or
									  gts_is_same(decay_t<CT>, std::string) or
									  gts_is_same(CT, const char*) or
									  gts_is_same(CT, char*);
	};
	template <typename CT, typename T = void>
	using not_type_t = enable_if_t<not is_string<CT>::value, T>;

	using base_type = std::string;
	// using base_type::basic_string; // Not available in gcc ...

public:
	value() = default;
	value(const std::string &str);
	value(const gts::value &other) = default;
	value(std::string &&str) noexcept;
	value(gts::value &&other) noexcept;
	value(const char *str, size_type len);

	template <typename T, typename U = not_type_t<T,int>>
	value(T &&v);

public:
	GTS_CXX_NODISCARD("") bool        to_bool   () const;
	GTS_CXX_NODISCARD("") int32_t     to_int    () const;
	GTS_CXX_NODISCARD("") uint32_t    to_uint   () const;
	GTS_CXX_NODISCARD("") int64_t     to_long   () const;
	GTS_CXX_NODISCARD("") uint64_t    to_ulong  () const;
	GTS_CXX_NODISCARD("") float       to_float  () const;
	GTS_CXX_NODISCARD("") double      to_double () const;
	GTS_CXX_NODISCARD("") long double to_ldouble() const;

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, bool), int)>
	GTS_CXX_NODISCARD("") bool get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, signed char), int)>
	GTS_CXX_NODISCARD("") signed char get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned char), int)>
	GTS_CXX_NODISCARD("") unsigned char get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, short), int)>
	GTS_CXX_NODISCARD("") short get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned short), int)>
	GTS_CXX_NODISCARD("") unsigned short get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, int), int)>
	GTS_CXX_NODISCARD("") int get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned int), int)>
	GTS_CXX_NODISCARD("") unsigned int get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long), int)>
	GTS_CXX_NODISCARD("") long get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long), int)>
	GTS_CXX_NODISCARD("") unsigned long get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long long), int)>
	GTS_CXX_NODISCARD("") long long get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long long), int)>
	GTS_CXX_NODISCARD("") unsigned long long get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, float), int)>
	GTS_CXX_NODISCARD("") float get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, double), int)>
	GTS_CXX_NODISCARD("") double get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long double), int)>
	GTS_CXX_NODISCARD("") long double get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)>
	GTS_CXX_NODISCARD("") std::string &get();

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)>
	GTS_CXX_NODISCARD("") const std::string &get() const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_enum(T), int)>
	GTS_CXX_NODISCARD("") T get() const;

public:
	value &set_value(const std::string &v);
	value &set_value(std::string &&v);

	template <typename...Args>
	value &set_value(fmt::format_string<Args...> fmt_value, Args&&...args);

	template <typename T, typename U = not_type_t<T,int>>
	value &set_value(T &&v);

public:
	template <typename...Args>
	GTS_CXX_NODISCARD("") static value from(fmt::format_string<Args...> fmt_value, Args&&...args);

	template <typename T, typename U = not_type_t<T,int>>
	GTS_CXX_NODISCARD("") static value from(T &&v);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_arithmetic(decay_t<T>),int)>
	value &operator=(T value);

public:
	value &operator=(const std::string &other);
	value &operator=(const gts::value &other) = default;
	value &operator=(std::string &&other) noexcept;
	value &operator=(gts::value &&other) noexcept;
};

using value_list = std::deque<gts::value>;

} //namespace gts

namespace std
{

template<> struct hash<gts::value> : public __hash_base<size_t, gts::value> {
	size_t operator()(const gts::value &v) const noexcept;
};

GTS_NAMESPACE_END
#include <gts/detail/value.h>

#endif //GTS_VALUE_H
