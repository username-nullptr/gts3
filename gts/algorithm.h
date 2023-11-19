
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

#ifndef GTS_ALGORITHM_H
#define GTS_ALGORITHM_H

#include <gts/string_list.h>
#include <rttr/variant.h>
#include <unordered_map>

GTS_NAMESPACE_BEGIN

GTSCORE_API GTS_CXX_NODISCARD("") int8_t   stoi8  (const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") uint8_t  stoui8 (const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") int16_t  stoi16 (const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") uint16_t stoui16(const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") int32_t  stoi32 (const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") uint32_t stoui32(const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") int64_t  stoi64 (const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") uint64_t stoui64(const std::string &str, int base = 10);
GTSCORE_API GTS_CXX_NODISCARD("") float    stof   (const std::string &str);
GTSCORE_API GTS_CXX_NODISCARD("") double   stod   (const std::string &str);
GTSCORE_API GTS_CXX_NODISCARD("") double   stold  (const std::string &str);
GTSCORE_API GTS_CXX_NODISCARD("") bool     stob   (const std::string &str);

GTSCORE_API GTS_CXX_NODISCARD("")
string_list str_split(const std::string &str, const std::string &splits, bool ignore_empty = true);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string str_list_join(const basic_string_list &list, const std::string &splits = " ");

GTSCORE_API GTS_CXX_NODISCARD("")
bool str_starts_with(const std::string &str, const std::string &prefix);

GTSCORE_API GTS_CXX_NODISCARD("")
bool str_ends_with(const std::string &str, const std::string &suffix);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string str_to_lower(const std::string &str);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string str_to_upper(const std::string &str);

GTSCORE_API /* GTS_CXX_NODISCARD("") */
std::size_t str_replace(std::string &str, const std::string &_old, const std::string &_new);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string str_trimmed(const std::string &str);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string str_remove(const std::string &str, const std::string &find);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string str_remove(const std::string &str, char find);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string from_percent_encoding(const std::string &str);

GTS_CXX_NODISCARD("") inline
string_list str_split(const std::string &str, char splits, bool ignore_empty = true)
{ return str_split(str, std::string(1,splits), ignore_empty); }

GTSCORE_API GTS_CXX_NODISCARD("")
std::string file_name(const std::string &file_name);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string file_path(const std::string &file_name);

GTS_NAMESPACE_END


#endif //GTS_ALGORITHM_H
