
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

#ifndef GTS_DBI_CELL_DETAIL_H
#define GTS_DBI_CELL_DETAIL_H

#ifndef _MSVC
# include <cxxabi.h>
#endif //VS

namespace gts { namespace dbi
{

inline cell::cell(const std::string &column_name) :
	m_column_name(column_name)
{

}

inline cell::cell(const std::string &column_name, std::string data) :
	m_column_name(column_name), m_data(std::move(data))
{

}

inline cell::cell(cell &&other) noexcept :
	m_column_name(std::move(other.m_column_name)), m_data(std::move(other.m_data))
{

}

inline cell &cell::operator=(cell &&other) noexcept
{
	m_column_name = std::move(other.m_column_name);
	m_data = std::move(other.m_data);
	return *this;
}

inline std::string cell::column_name() const
{
	return m_column_name;
}

inline bool cell::has_value() const
{
	return m_data.has_value();
}

inline bool cell::is_valid() const
{
	return has_value();
}

inline bool cell::not_has_value() const
{
	return not has_value();
}

inline bool cell::is_null() const
{
	return not_has_value();
}

template <typename T, typename _GTEI_0>
bool cell::get(T default_value) const noexcept
{
	if( has_value() )
		return to_bool(get<std::string>(default_value? "true" : "false"));
	return default_value;
}

template <typename T, typename _GTEI_0>
bool cell::get() const noexcept(false)
{
	if( has_value() )
		return to_bool(get<std::string>());
	throw exception(-1, "dbi::cell_data::get<bool>: Optional is empty.");
	return false;
}

template <typename T, typename _GTEI_0>
signed char cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtol, default_value);
}

template <typename T, typename _GTEI_0>
signed char cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtol);
}

template <typename T, typename _GTEI_0>
unsigned char cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtol, default_value);
}

template <typename T, typename _GTEI_0>
unsigned char cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtol);
}

template <typename T, typename _GTEI_0>
short cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtol, default_value);
}

template <typename T, typename _GTEI_0>
short cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtol);
}

template <typename T, typename _GTEI_0>
unsigned short cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtol, default_value);
}

template <typename T, typename _GTEI_0>
unsigned short cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtol);
}

template <typename T, typename _GTEI_0>
int cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtol, default_value);
}

template <typename T, typename _GTEI_0>
int cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtol);
}

template <typename T, typename _GTEI_0>
unsigned int cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtoul, default_value);
}

template <typename T, typename _GTEI_0>
unsigned int cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtoul);
}

template <typename T, typename _GTEI_0>
long cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtol, default_value);
}

template <typename T, typename _GTEI_0>
long cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtol);
}

template <typename T, typename _GTEI_0>
unsigned long cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtoul, default_value);
}

template <typename T, typename _GTEI_0>
unsigned long cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtoul);
}

template <typename T, typename _GTEI_0>
long long cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtoll, default_value);
}

template <typename T, typename _GTEI_0>
long long cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtoll);
}

template <typename T, typename _GTEI_0>
unsigned long long cell::get(T default_value, int base) const noexcept
{
	return _get<T>(base, std::strtoull, default_value);
}

template <typename T, typename _GTEI_0>
unsigned long long cell::get(int base) const noexcept(false)
{
	return _get<T>(base, std::strtoull);
}

template <typename T, typename _GTEI_0>
float cell::get(T default_value) const noexcept
{
	return _get<T>(std::strtof, default_value);
}

template <typename T, typename _GTEI_0>
float cell::get() const noexcept(false)
{
	return _get<T>(std::strtof);
}

template <typename T, typename _GTEI_0>
double cell::get(T default_value) const noexcept
{
	return _get<T>(std::strtod, default_value);
}

template <typename T, typename _GTEI_0>
double cell::get() const noexcept(false)
{
	return _get<T>(std::strtod);
}

template <typename T, typename _GTEI_0>
long double cell::get(T default_value) const noexcept
{
	return _get<T>(std::strtold, default_value);
}

template <typename T, typename _GTEI_0>
long double cell::get() const noexcept(false)
{
	return _get<T>(std::strtold);
}

template <typename T, typename _GTEI_0>
std::string cell::get(const T &default_value) const noexcept
{
	return m_data.value_or(default_value);
}

template <typename T, typename _GTEI_0>
std::string cell::get(T &&default_value) const noexcept
{
	return m_data.value_or(std::move(default_value));
}

template <typename T, typename _GTEI_0>
const std::string &cell::get() const noexcept(false)
{
	if( has_value() )
		return m_data.value();
	throw exception(-1, "dbi::cell_data::get<std::string>: Optional is empty.");
}

template <typename T, typename _GTEI_0>
std::string &cell::get() noexcept(false)
{
	if( has_value() )
		return m_data.value();
	throw exception(-1, "dbi::cell_data::get<std::string>: Optional is empty.");
}

template <typename T, typename _GTEI_0>
T cell::get(T default_value) const noexcept
{
	return static_cast<T>(get<int>(static_cast<int>(default_value)));
}

template <typename T, typename _GTEI_0>
T cell::get() const noexcept(false)
{
	return static_cast<T>(get<int>());
}

inline bool cell::to_bool(bool default_value) const noexcept
{
	return get<bool>(default_value);
}

inline bool cell::to_bool() const noexcept(false)
{
	if( has_value() )
		return to_bool(get<std::string>());
	throw exception(-1, "dbi::cell_data::to_bool: Optional is empty.");
	return 0;
}

inline int8_t cell::to_char(int8_t default_value) const noexcept
{
	return get<int8_t>(default_value);
}

inline int8_t cell::to_char() const noexcept(false)
{
	if( has_value() )
		return get<int8_t>();
	throw exception(-1, "dbi::cell_data::to_char: Optional is empty.");
	return 0;
}

inline uint8_t cell::to_uchar(uint8_t default_value) const noexcept
{
	return get<uint8_t>(default_value);
}

inline uint8_t cell::to_uchar() const noexcept(false)
{
	if( has_value() )
		return get<uint8_t>();
	throw exception(-1, "dbi::cell_data::to_uchar: Optional is empty.");
	return 0;
}

inline int16_t cell::to_short(int16_t default_value) const noexcept
{
	return get<int16_t>(default_value);
}

inline int16_t cell::to_short() const noexcept(false)
{
	if( has_value() )
		return get<int16_t>();
	throw exception(-1, "dbi::cell_data::to_short: Optional is empty.");
	return 0;
}

inline uint16_t cell::to_ushort(uint16_t default_value) const noexcept
{
	return get<uint16_t>(default_value);
}

inline uint16_t cell::to_ushort() const noexcept(false)
{
	if( has_value() )
		return get<uint16_t>();
	throw exception(-1, "dbi::cell_data::to_ushort: Optional is empty.");
	return 0;
}

inline int32_t cell::to_int(int32_t default_value) const noexcept
{
	return get<int32_t>(default_value);
}

inline int32_t cell::to_int() const noexcept(false)
{
	if( has_value() )
		return get<int32_t>();
	throw exception(-1, "dbi::cell_data::to_int: Optional is empty.");
	return 0;
}

inline uint32_t cell::to_uint(uint32_t default_value) const noexcept
{
	return get<uint32_t>(default_value);
}

inline uint32_t cell::to_uint() const noexcept(false)
{
	if( has_value() )
		return get<uint32_t>();
	throw exception(-1, "dbi::cell_data::to_uint: Optional is empty.");
	return 0;
}

inline int64_t cell::to_long(int64_t default_value) const noexcept
{
	return get<int64_t>(default_value);
}

inline int64_t cell::to_long() const noexcept(false)
{
	if( has_value() )
		return get<int64_t>();
	throw exception(-1, "dbi::cell_data::to_long: Optional is empty.");
	return 0;
}

inline uint64_t cell::to_ulong(uint64_t default_value) const noexcept
{
	return get<uint64_t>(default_value);
}

inline uint64_t cell::to_ulong() const noexcept(false)
{
	if( has_value() )
		return get<uint64_t>();
	throw exception(-1, "dbi::cell_data::to_ulong: Optional is empty.");
	return 0;
}

inline float cell::to_float(float default_value) const noexcept
{
	return get<float>(default_value);
}

inline float cell::to_float() const noexcept(false)
{
	if( has_value() )
		return get<float>();
	throw exception(-1, "dbi::cell_data::to_float: Optional is empty.");
	return 0.0;
}

inline double cell::to_double(double default_value) const noexcept
{
	return get<double>(default_value);
}

inline double cell::to_double() const noexcept(false)
{
	if( has_value() )
		return get<double>();
	throw exception(-1, "dbi::cell_data::to_double: Optional is empty.");
	return 0.0;
}

inline long double cell::to_long_double(long double default_value) const noexcept
{
	return get<long double>(default_value);
}

inline long double cell::to_long_double() const noexcept(false)
{
	if( has_value() )
		return get<long double>();
	throw exception(-1, "dbi::cell_data::to_long_double: Optional is empty.");
	return 0.0;
}

inline std::string cell::to_string(const std::string &default_value) const noexcept
{
	return get<std::string>(default_value);
}

inline std::string cell::to_string(std::string &&default_value) const noexcept
{
	return get<std::string>(std::move(default_value));
}

inline const std::string &cell::to_string() const noexcept(false)
{
	if( has_value() )
		return get<std::string>();
	throw exception(-1, "dbi::cell_data::to_string: Optional is empty.");
}

inline std::string &cell::to_string() noexcept(false)
{
	if( has_value() )
		return get<std::string>();
	throw exception(-1, "dbi::cell_data::to_string: Optional is empty.");
}

inline cell::operator const std::string() const noexcept(false)
{
	return to_string();
}

inline cell::operator std::string() noexcept(false)
{
	return to_string();
}

inline bool cell::operator==(const cell &other) const noexcept
{
	return has_value() and other.has_value() and m_data.value() == other.m_data.value();
}

inline bool cell::operator!=(const cell &other) const noexcept
{
	return not operator==(other);
}

inline bool cell::operator<(const cell &other) const noexcept(false)
{
	return to_string() < other.to_string();
}

inline bool cell::operator<=(const cell &other) const noexcept(false)
{
	return to_string() <= other.to_string();
}

inline bool cell::operator>(const cell &other) const noexcept(false)
{
	return to_string() > other.to_string();
}

inline bool cell::operator>=(const cell &other) const noexcept(false)
{
	return to_string() >= other.to_string();
}

inline bool cell::to_bool(const std::string &str) const noexcept
{
#ifdef __unix
	if( str.size() == 4 and strncasecmp(str.c_str(), "true", 4) )
		return true;
	else if( str.size() == 5 and strncasecmp(str.c_str(), "false", 5) )
		return false;
#else
	if( stricmp(str.c_str(), "true") )
		return true;
	else if( stricmp(str.c_str(), "false") )
		return false;
#endif
	try {
		return std::stoi(str);
	} catch(...) {}
	return false;
}

template <typename T, typename F>
T cell::_get(int base, F &&func, T default_value) const noexcept
{
	if( has_value() )
	{
		char *end = nullptr;
		if( base < 2 )
			base = 2;
		else if( base > 16 )
			base = 16;
		return func(m_data.value().c_str(), &end, base);
	}
	return default_value;
}

template <typename T, typename F>
T cell::_get(F &&func, T default_value) const noexcept
{
	if( has_value() )
	{
		char *end = nullptr;
		return func(m_data.value().c_str(), &end);
	}
	return default_value;
}

template <typename T, typename F>
T cell::_get(int base, F &&func) const noexcept(false)
{
	if( has_value() )
	{
		char *end = nullptr;
		if( base < 2 )
			base = 2;
		else if( base > 16 )
			base = 16;
		return func(m_data.value().c_str(), &end, base);
	}
	throw exception(-1, std::string("dbi::cell_data::get<") +
				#ifdef _MSVC
					typeid(T).name
				#else
					abi::__cxa_demangle(typeid(T).name(), 0,0,0)
				#endif
					+ ">: Optional is empty.");
	return T();
}

template <typename T, typename F>
T cell::_get(F &&func) const noexcept(false)
{
	if( has_value() )
	{
		char *end = nullptr;
		return func(m_data.value().c_str(), &end);
	}
	throw exception(-1, std::string("dbi::cell_data::get<") +
				#ifdef _MSVC
					typeid(T).name
				#else
					abi::__cxa_demangle(typeid(T).name(), 0,0,0)
				#endif
					+ ">: Optional is empty.");
	return T();
}

inline bool operator==(const cell &cell, const std::string &str)
{
	return cell.has_value() and cell.to_string() == str;
}

inline bool operator!=(const cell &cell, const std::string &str)
{
	return not operator==(cell, str);
}

inline bool operator>(const cell &cell, const std::string &str)
{
	return cell.to_string() > str;
}

inline bool operator<(const cell &cell, const std::string &str)
{
	return cell.to_string() < str;
}

inline bool operator>=(const cell &cell, const std::string &str)
{
	return cell.to_string() >= str;
}

inline bool operator<=(const cell &cell, const std::string &str)
{
	return cell.to_string() <= str;
}

inline bool operator==(const std::string &str, const cell &cell)
{
	return cell.has_value() and cell.to_string() == str;
}

inline bool operator!=(const std::string &str, const cell &cell)
{
	return not operator==(cell, str);
}

inline bool operator>(const std::string &str, const cell &cell)
{
	return str > cell.to_string();
}

inline bool operator<(const std::string &str, const cell &cell)
{
	return str < cell.to_string();
}

inline bool operator>=(const std::string &str, const cell &cell)
{
	return str >= cell.to_string();
}

inline bool operator<=(const std::string &str, const cell &cell)
{
	return str <= cell.to_string();
}

inline std::string operator+(const cell &cell, const std::string &str)
{
	return cell.has_value() ? cell.to_string() + str : str;
}

inline std::string operator+(const std::string &str, const cell &cell)
{
	return cell.has_value() ? str + cell.to_string() : str;
}

}} //namespace gts::dbi


#endif //GTS_DBI_CELL_DETAIL_H
