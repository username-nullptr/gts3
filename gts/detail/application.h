
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

#ifndef GTS_DETAIL_APPLICATION_H
#define GTS_DETAIL_APPLICATION_H

namespace gts
{

template <typename...Args>
bool setenv(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	return setenv(key, fmt::format(fmt_value, std::forward<Args>(args)...));
}

template <typename...Args>
bool setenv(const std::string &key, bool overwrite, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	return setenv(key, fmt::format(fmt_value, std::forward<Args>(args)...), overwrite);
}

template <typename T, typename _U>
bool setenv(const std::string &key, T &&value, bool overwrite = true)
{
	return setenv(key, fmt::format("{}", std::forward<T>(value)), overwrite);
}

} //namespace gts


#endif //GTS_DETAIL_APPLICATION_H
