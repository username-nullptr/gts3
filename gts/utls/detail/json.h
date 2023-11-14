
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

#ifndef GTS_UTLS_DETAIL_JSON_H
#define GTS_UTLS_DETAIL_JSON_H

namespace gts { namespace njson_utls
{

template <typename T>
void to_struct(const njson &json, T&t) {
	from_json_recursively(t, json);
}

template <typename T>
T to_struct(const njson &json)
{
	T obj;
	to_struct(json, obj);
	return obj;
}

template <typename T>
void parse_to_struct(const std::string &str, T &obj) {
	to_struct(njson::parse(str), obj);
}

template <typename T>
T parse_to_struct(const std::string &str)
{
	T obj;
	parse_to_struct(str, obj);
	return obj;
}

template <typename T>
njson from_struct(const T &obj)
{
	njson json(njson_type::object);
	to_json_recursively(obj, json);
	return json;
}

}} //namespace gts::njson_utls


#endif //GTS_UTLS_DETAIL_JSON_H
