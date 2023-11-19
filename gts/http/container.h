
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

#ifndef GTS_HTTP_CONTAINER_H
#define GTS_HTTP_CONTAINER_H

#include <gts/value.h>
#include <gts/http/global.h>
#include <unordered_map>
#include <map>

GTS_HTTP_NAMESPACE_BEGIN

template <typename T = gts::value, GTS_TYPE_ENABLE_IF(gts_is_base_of(gts::value, T), int)>
using pair = std::pair<std::string, T>;

template <typename V = gts::value, typename Compare = std::less<std::string>>
class map : public std::map<std::string, V, Compare>
{
	static_assert(gts_is_base_of(gts::value, V),
	"The template argument 'V' must be a 'gts::http::value' or derived class of 'gts::value'.");

public:
	using base_type = std::map<std::string, V, Compare>;
	using base_type::base_type;

public:
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, const std::string &value);
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, std::string &&value);

	template <typename T, typename U = gts::value::not_type_t<T,int>>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, T &&value);

	template <typename...Args>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args);

public:
	GTS_CXX_NODISCARD("") V value(const std::string &key, const V &default_value) const;
	GTS_CXX_NODISCARD("") bool value_bool(const std::string &key, bool default_value) const;

	GTS_CXX_NODISCARD("") int32_t value_int(const std::string &key, int32_t default_value) const;
	GTS_CXX_NODISCARD("") uint32_t value_uint(const std::string &key, uint32_t default_value) const;

	GTS_CXX_NODISCARD("") int64_t value_long(const std::string &key, int64_t default_value) const;
	GTS_CXX_NODISCARD("") uint64_t value_ulong(const std::string &key, uint64_t default_value) const;

	GTS_CXX_NODISCARD("") float value_float(const std::string &key, float default_value) const;
	GTS_CXX_NODISCARD("") double value_double(const std::string &key, double default_value) const;
	GTS_CXX_NODISCARD("") long double value_ldouble(const std::string &key, long double default_value) const;

public:
	GTS_CXX_NODISCARD("") V &value(const std::string &key);
	GTS_CXX_NODISCARD("") const V &value(const std::string &key) const;
	GTS_CXX_NODISCARD("") bool value_bool(const std::string &key) const;

	GTS_CXX_NODISCARD("") int32_t value_int(const std::string &key) const;
	GTS_CXX_NODISCARD("") uint32_t value_uint(const std::string &key) const;

	GTS_CXX_NODISCARD("") int64_t value_long(const std::string &key) const;
	GTS_CXX_NODISCARD("") uint64_t value_ulong(const std::string &key) const;

	GTS_CXX_NODISCARD("") float value_float(const std::string &key) const;
	GTS_CXX_NODISCARD("") double value_double(const std::string &key) const;
	GTS_CXX_NODISCARD("") long double value_ldouble(const std::string &key) const;

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_arithmetic(T), int)>
	GTS_CXX_NODISCARD("") T value(const std::string &key) const;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_arithmetic(T), int)>
	GTS_CXX_NODISCARD("") T value(const std::string &key, T default_value) const;

public:
	using base_type::operator[];
	const V &operator[](const std::string &key) const;
};

struct less_case_insensitive
{
	bool operator()(const std::string &v1, const std::string &v2) const;
	struct nocase_compare {
		bool operator()(unsigned char c1, unsigned char c2) const;
	};
};

template <typename V = gts::value>
class unordered_map : public std::unordered_map<std::string, V>
{
	static_assert(gts_is_base_of(gts::value, V),
	"The template argument 'V' must be a 'gts::http::value' or derived class of 'gts::http::value'.");

public:
	using base_type = std::unordered_map<std::string, V>;
	using base_type::base_type;

public:
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, const std::string &value);
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, std::string &&value);

	template <typename T, typename U = gts::value::not_type_t<T,int>>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, T &&value);

	template <typename...Args>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args);

public:
	GTS_CXX_NODISCARD("") V value(const std::string &key, const V &default_value) const;
	GTS_CXX_NODISCARD("") bool value_bool(const std::string &key, bool default_value) const;

	GTS_CXX_NODISCARD("") int32_t value_int(const std::string &key, int32_t default_value) const;
	GTS_CXX_NODISCARD("") uint32_t value_uint(const std::string &key, uint32_t default_value) const;

	GTS_CXX_NODISCARD("") int64_t value_long(const std::string &key, int64_t default_value) const;
	GTS_CXX_NODISCARD("") uint64_t value_ulong(const std::string &key, uint64_t default_value) const;

	GTS_CXX_NODISCARD("") float value_float(const std::string &key, float default_value) const;
	GTS_CXX_NODISCARD("") double value_double(const std::string &key, double default_value) const;
	GTS_CXX_NODISCARD("") long double value_ldouble(const std::string &key, long double default_value) const;

public:
	GTS_CXX_NODISCARD("") V &value(const std::string &key);
	GTS_CXX_NODISCARD("") const V &value(const std::string &key) const;
	GTS_CXX_NODISCARD("") bool value_bool(const std::string &key) const;

	GTS_CXX_NODISCARD("") int32_t value_int(const std::string &key) const;
	GTS_CXX_NODISCARD("") uint32_t value_uint(const std::string &key) const;

	GTS_CXX_NODISCARD("") int64_t value_long(const std::string &key) const;
	GTS_CXX_NODISCARD("") uint64_t value_ulong(const std::string &key) const;

	GTS_CXX_NODISCARD("") float value_float(const std::string &key) const;
	GTS_CXX_NODISCARD("") double value_double(const std::string &key) const;
	GTS_CXX_NODISCARD("") long double value_ldouble(const std::string &key) const;

public:
	using base_type::operator[];
	const V &operator[](const std::string &key) const;
};

GTS_HTTP_NAMESPACE_END
#include <gts/http/detail/container.h>

#endif //GTS_HTTP_CONTAINER_H
