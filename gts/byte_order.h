
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

#ifndef GTS_BYTE_ORDER_H
#define GTS_BYTE_ORDER_H

#include <gts/global.h>

GTS_NAMESPACE_BEGIN

GTS_CXX_NODISCARD("")
GTSCORE_API bool is_little_endian();

GTS_CXX_NODISCARD("")
inline bool is_big_endian();

template <typename T, GTS_TYPE_ENABLE_IF(gts_is_integral(T),int)>
GTS_CXX_NODISCARD("")
T hton(T t);

template <typename T>
GTS_CXX_NODISCARD("")
T *hton(T *data, std::size_t len);

template <typename T, GTS_TYPE_ENABLE_IF(gts_is_integral(T),int)>
GTS_CXX_NODISCARD("")
T ntoh(T t);

template <typename T>
GTS_CXX_NODISCARD("")
T *ntoh(T *data, std::size_t len);

template <typename T, GTS_TYPE_ENABLE_IF(gts_is_integral(T),int)>
GTS_CXX_NODISCARD("")
T reverse(T t);

template <typename T>
GTS_CXX_NODISCARD("")
T *reverse(T *data, std::size_t len);

GTS_NAMESPACE_END
#include <gts/detail/byte_order.h>


#endif //GTS_BYTE_ORDER_H
