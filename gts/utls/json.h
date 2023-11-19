
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

#ifndef GTS_UTLS_JSON_H
#define GTS_UTLS_JSON_H

#include <gts/utls/registration.h>
#include <nlohmann/json.hpp>

using njson = nlohmann::json;

using njson_type = njson::value_t;

GTS_NJSON_UTLS_NAMESPACE_BEGIN

GTS_UTLS_API void from_json_recursively(rttr::instance instance, const njson &json);

template <typename T>
void to_struct(const njson &json, T&t);

template <typename T>
T to_struct(const njson &json);

template <typename T>
void parse_to_struct(const std::string &str, T &obj);

template <typename T>
T parse_to_struct(const std::string &str);

GTS_UTLS_API void to_json_recursively(const rttr::instance instance, njson &json);

template <typename T>
njson from_struct(const T &obj);

GTS_NJSON_UTLS_NAMESPACE_END
#include <gts/utls/detail/json.h>

#endif //GTS_UTLS_JSON_H
