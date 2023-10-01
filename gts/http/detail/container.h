
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

#ifndef GTS_HTTP_DETAIL_CONTAINER_H
#define GTS_HTTP_DETAIL_CONTAINER_H

namespace gts { namespace http
{

template <typename V, typename _Compare>
std::pair<typename map<V,_Compare>::base_type::iterator, bool>
map<V,_Compare>::emplace(const std::string &key, const std::string &value)
{
	return base_type::emplace(key, V(value));
}

template <typename V, typename _Compare>
std::pair<typename map<V,_Compare>::base_type::iterator, bool>
map<V,_Compare>::emplace(const std::string &key, std::string &&value)
{
	return base_type::emplace(key, V(std::move(value)));
}

template <typename V, typename _Compare>
template <typename T, typename U>
std::pair<typename map<V,_Compare>::base_type::iterator, bool>
map<V,_Compare>::emplace(const std::string &key, T &&value)
{
	return base_type::emplace(key, V(std::forward<T>(value)));
}

template <typename V, typename _Compare>
template <typename...Args>
std::pair<typename map<V,_Compare>::base_type::iterator, bool>
map<V,_Compare>::emplace(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	return base_type::emplace(key, V(fmt_value, std::forward<Args>(args)...));
}

template <typename V, typename _Compare>
V map<V,_Compare>::value(const std::string &key, const V &default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second;
}

template <typename V, typename _Compare>
bool map<V,_Compare>::value_bool(const std::string &key, bool default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_bool();
}

template <typename V, typename _Compare>
int32_t map<V,_Compare>::value_int(const std::string &key, int32_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_int();
}

template <typename V, typename _Compare>
uint32_t map<V,_Compare>::value_uint(const std::string &key, uint32_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_uint();
}

template <typename V, typename _Compare>
int64_t map<V,_Compare>::value_long(const std::string &key, int64_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_long();
}

template <typename V, typename _Compare>
uint64_t map<V,_Compare>::value_ulong(const std::string &key, uint64_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_ulong();
}

template <typename V, typename _Compare>
float map<V,_Compare>::value_float(const std::string &key, float default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_float();
}

template <typename V, typename _Compare>
double map<V,_Compare>::value_double(const std::string &key, double default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_double();
}

template <typename V, typename _Compare>
long double map<V,_Compare>::value_ldouble(const std::string &key, long double default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_ldouble();
}

template <typename V, typename _Compare>
V &map<V,_Compare>::value(const std::string &key)
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second;
}

template <typename V, typename _Compare>
const V &map<V,_Compare>::value(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second;
}

template <typename V, typename _Compare>
bool map<V,_Compare>::value_bool(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_bool();
}

template <typename V, typename _Compare>
int32_t map<V,_Compare>::value_int(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_int();
}

template <typename V, typename _Compare>
uint32_t map<V,_Compare>::value_uint(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_uint();
}

template <typename V, typename _Compare>
int64_t map<V,_Compare>::value_long(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_long();
}

template <typename V, typename _Compare>
uint64_t map<V,_Compare>::value_ulong(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_ulong();
}

template <typename V, typename _Compare>
float map<V,_Compare>::value_float(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_float();
}

template <typename V, typename _Compare>
double map<V,_Compare>::value_double(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_double();
}

template <typename V, typename _Compare>
long double map<V,_Compare>::value_ldouble(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_ldouble();
}

template <typename V, typename _Compare>
template <typename T, typename _GTEI_0>
T map<V,_Compare>::value(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.template get<T>();
}

template <typename V, typename _Compare>
template <typename T, typename _GTEI_0>
T map<V,_Compare>::value(const std::string &key, T default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.template get<T>();
}

template <typename V, typename _Compare>
const V &map<V,_Compare>::operator[](const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second;
}

inline bool less_case_insensitive::operator()(const std::string &v1, const std::string &v2) const
{
	return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end(), nocase_compare());
}

inline bool less_case_insensitive::nocase_compare::operator()(const unsigned char c1, const unsigned char c2) const
{
	return tolower(c1) < tolower(c2);
}

template <typename V>
std::pair<typename unordered_map<V>::base_type::iterator, bool>
unordered_map<V>::emplace(const std::string &key, const std::string &value)
{
	return base_type::emplace(key, V(value));
}

template <typename V>
std::pair<typename unordered_map<V>::base_type::iterator, bool>
unordered_map<V>::emplace(const std::string &key, std::string &&value)
{
	return base_type::emplace(key, V(std::move(value)));
}

template <typename V>
template <typename T, typename U>
std::pair<typename unordered_map<V>::base_type::iterator, bool>
unordered_map<V>::emplace(const std::string &key, T &&value)
{
	return base_type::emplace(key, V(std::forward<T>(value)));
}

template <typename V>
template <typename...Args>
std::pair<typename unordered_map<V>::base_type::iterator, bool>
unordered_map<V>::emplace(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	return base_type::emplace(key, V(fmt_value, std::forward<Args>(args)...));
}

template <typename V>
V unordered_map<V>::value(const std::string &key, const V &default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second;
}

template <typename V>
bool unordered_map<V>::value_bool(const std::string &key, bool default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_bool();
}

template <typename V>
int32_t unordered_map<V>::value_int(const std::string &key, int32_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_int();
}

template <typename V>
uint32_t unordered_map<V>::value_uint(const std::string &key, uint32_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_uint();
}

template <typename V>
int64_t unordered_map<V>::value_long(const std::string &key, int64_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_long();
}

template <typename V>
uint64_t unordered_map<V>::value_ulong(const std::string &key, uint64_t default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_ulong();
}

template <typename V>
float unordered_map<V>::value_float(const std::string &key, float default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_float();
}

template <typename V>
double unordered_map<V>::value_double(const std::string &key, double default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_double();
}

template <typename V>
long double unordered_map<V>::value_ldouble(const std::string &key, long double default_value) const
{
	auto it = this->find(key);
	return it == this->end()? default_value : it->second.to_ldouble();
}

template <typename V>
V &unordered_map<V>::value(const std::string &key)
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second;
}

template <typename V>
const V &unordered_map<V>::value(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second;
}

template <typename V>
bool unordered_map<V>::value_bool(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_bool();
}

template <typename V>
int32_t unordered_map<V>::value_int(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_int();
}

template <typename V>
uint32_t unordered_map<V>::value_uint(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_uint();
}

template <typename V>
int64_t unordered_map<V>::value_long(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_long();
}

template <typename V>
uint64_t unordered_map<V>::value_ulong(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_ulong();
}

template <typename V>
float unordered_map<V>::value_float(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_float();
}

template <typename V>
double unordered_map<V>::value_double(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_double();
}

template <typename V>
long double unordered_map<V>::value_ldouble(const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second.to_ldouble();
}

template <typename V>
const V &unordered_map<V>::operator[](const std::string &key) const
{
	auto it = this->find(key);
	assert(it != this->end());
	return it->second;
}

}} //namespace gts::http


#endif //GTS_HTTP_DETAIL_CONTAINER_H
