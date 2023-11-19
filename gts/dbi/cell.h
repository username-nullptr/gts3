
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

#ifndef GTS_DBI_CELL_H
#define GTS_DBI_CELL_H

#include <gts/dbi/error.h>
#include <cppoptional>
#include <cstring>

GTS_DBI_NAMESPACE_BEGIN

class GTS_DBI_API cell
{
public:
	cell() = default;
	cell(std::string column_name);
	cell(std::string column_name, std::string data);

public:
	cell(const cell &other) = default;
	cell(cell &&other) noexcept;
	cell &operator=(const cell &other) = default;
	cell &operator=(cell &&other) noexcept;

public:
	GTS_CXX_NODISCARD("") std::string column_name() const;

public:
	GTS_CXX_NODISCARD("") bool has_value() const;
	GTS_CXX_NODISCARD("") bool is_valid() const;
	GTS_CXX_NODISCARD("") bool not_has_value() const;
	GTS_CXX_NODISCARD("") bool is_null() const;

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, bool), int)> GTS_CXX_NODISCARD("")
	bool get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, bool), int)> GTS_CXX_NODISCARD("")
	bool get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, signed char), int)> GTS_CXX_NODISCARD("")
	signed char get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, signed char), int)> GTS_CXX_NODISCARD("")
	signed char get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned char), int)> GTS_CXX_NODISCARD("")
	unsigned char get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned char), int)> GTS_CXX_NODISCARD("")
	unsigned char get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, short), int)> GTS_CXX_NODISCARD("")
	short get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, short), int)> GTS_CXX_NODISCARD("")
	short get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned short), int)> GTS_CXX_NODISCARD("")
	unsigned short get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned short), int)> GTS_CXX_NODISCARD("")
	unsigned short get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, int), int)> GTS_CXX_NODISCARD("")
	int get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, int), int)> GTS_CXX_NODISCARD("")
	int get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned int), int)> GTS_CXX_NODISCARD("")
	unsigned int get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned int), int)> GTS_CXX_NODISCARD("")
	unsigned int get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long), int)> GTS_CXX_NODISCARD("")
	long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long), int)> GTS_CXX_NODISCARD("")
	long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long), int)> GTS_CXX_NODISCARD("")
	unsigned long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long), int)> GTS_CXX_NODISCARD("")
	unsigned long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long long), int)> GTS_CXX_NODISCARD("")
	long long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long long), int)> GTS_CXX_NODISCARD("")
	long long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long long), int)> GTS_CXX_NODISCARD("")
	unsigned long long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long long), int)> GTS_CXX_NODISCARD("")
	unsigned long long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, float), int)> GTS_CXX_NODISCARD("")
	float get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, float), int)> GTS_CXX_NODISCARD("")
	float get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, double), int)> GTS_CXX_NODISCARD("")
	double get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, double), int)> GTS_CXX_NODISCARD("")
	double get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long double), int)> GTS_CXX_NODISCARD("")
	long double get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long double), int)> GTS_CXX_NODISCARD("")
	long double get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)> GTS_CXX_NODISCARD("")
	std::string get(const T &default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)> GTS_CXX_NODISCARD("")
	std::string get(T &&default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)> GTS_CXX_NODISCARD("")
	const std::string &get() const noexcept(false);

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)> GTS_CXX_NODISCARD("")
	std::string &get() noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_enum(T), int)> GTS_CXX_NODISCARD("")
	T get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_enum(T), int)> GTS_CXX_NODISCARD("")
	T get() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") bool to_bool(bool default_value) const noexcept;
	GTS_CXX_NODISCARD("") bool to_bool() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") int8_t to_char(int8_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") int8_t to_char() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") uint8_t to_uchar(uint8_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") uint8_t to_uchar() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") int16_t to_short(int16_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") int16_t to_short() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") uint16_t to_ushort(uint16_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") uint16_t to_ushort() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") int32_t to_int(int32_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") int32_t to_int() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") uint32_t to_uint(uint32_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") uint32_t to_uint() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") int64_t to_long(int64_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") int64_t to_long() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") uint64_t to_ulong(uint64_t default_value) const noexcept;
	GTS_CXX_NODISCARD("") uint64_t to_ulong() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") float to_float(float default_value) const noexcept;
	GTS_CXX_NODISCARD("") float to_float() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") double to_double(double default_value) const noexcept;
	GTS_CXX_NODISCARD("") double to_double() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") long double to_long_double(long double default_value) const noexcept;
	GTS_CXX_NODISCARD("") long double to_long_double() const noexcept(false);

public:
	GTS_CXX_NODISCARD("") std::string to_string(const std::string &default_value) const noexcept;
	GTS_CXX_NODISCARD("") std::string to_string(std::string &&default_value) const noexcept;
	GTS_CXX_NODISCARD("") const std::string &to_string() const noexcept(false);
	GTS_CXX_NODISCARD("") std::string &to_string() noexcept(false);

public:
	operator const std::string() const noexcept(false);
	operator std::string() noexcept(false);

public:
	GTS_CXX_NODISCARD("") bool operator==(const cell &other) const noexcept;
	GTS_CXX_NODISCARD("") bool operator!=(const cell &other) const noexcept;
	GTS_CXX_NODISCARD("") bool operator<(const cell &other) const noexcept(false);
	GTS_CXX_NODISCARD("") bool operator<=(const cell &other) const noexcept(false);
	GTS_CXX_NODISCARD("") bool operator>(const cell &other) const noexcept(false);
	GTS_CXX_NODISCARD("") bool operator>=(const cell &other) const noexcept(false);

private:
	GTS_CXX_NODISCARD("") static
	bool to_bool(const std::string &str) noexcept;

private:
	template <typename T, typename F>
	T _get(int base, F &&func, T default_value) const noexcept;

	template <typename T, typename F>
	T _get(F &&func, T default_value) const noexcept;

	template <typename T, typename F>
	T _get(int base, F &&func) const noexcept(false);

	template <typename T, typename F>
	T _get(F &&func) const noexcept(false);

private:
	std::string m_column_name = "NULL";
	cpp::optional<std::string> m_data;
};

GTS_DBI_NAMESPACE_END
#include <gts/dbi/detail/cell.h>

#endif //GTS_DBI_CELL_H
