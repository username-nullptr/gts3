
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

#ifndef GTS_APPLICATION_H
#define GTS_APPLICATION_H

#include <gts/global.h>
#include <gts/formatter.h>
#include <cppoptional>

GTS_APP_NAMESPACE_BEGIN

GTSCORE_API GTS_CXX_NODISCARD("")
std::string instance_name();

GTSCORE_API GTS_CXX_NODISCARD("")
std::string file_path();

GTSCORE_API GTS_CXX_NODISCARD("")
std::string dir_path();

GTSCORE_API /* GTS_CXX_NODISCARD("") */
bool set_current_directory(const std::string &path);

GTSCORE_API GTS_CXX_NODISCARD("")
std::string current_directory();

GTSCORE_API GTS_CXX_NODISCARD("")
std::string absolute_path(const std::string &path);

GTSCORE_API GTS_CXX_NODISCARD("")
bool is_absolute_path(const std::string &path);

GTSCORE_API GTS_CXX_NODISCARD("")
cpp::optional<std::string> getenv(const std::string &key);

GTSCORE_API GTS_CXX_NODISCARD("")
std::map<std::string, std::string> getenvs();

GTSCORE_API /* GTS_CXX_NODISCARD("") */
bool setenv(const std::string &key, const std::string &value, bool overwrite = true);

GTSCORE_API /* GTS_CXX_NODISCARD("") */
bool unsetenv(const std::string &key);

template <typename...Args> /* GTS_CXX_NODISCARD("") */
bool setenv(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args);

template <typename...Args> /* GTS_CXX_NODISCARD("") */
bool setenv(const std::string &key, bool overwrite, fmt::format_string<Args...> fmt_value, Args&&...args);

template <typename T, typename _U = enable_if_t<not gts_is_same(decay_t<T>, std::string) and
												not gts_is_same(T, const char*) and
												not gts_is_same(T, char*), int>> /* GTS_CXX_NODISCARD("") */
bool setenv(const std::string &key, T &&value, bool overwrite = true);

GTS_APP_NAMESPACE_END
#include <gts/detail/application.h>

#endif //GTS_APPLICATION_H
