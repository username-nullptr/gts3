
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

#ifndef GTS_CPP_ATTRIBUTE_H
#define GTS_CPP_ATTRIBUTE_H

#ifdef _MSC_VER

# define GTS_DECL_EXPORT  __declspec(dllexport)
# define GTS_DECL_IMPORT  __declspec(dllimport)
# define GTS_DECL_HIDDEN

# define __attribute_used__    __declspec(used)
# define __attribute_unused__  __declspec(unused)

# define __attribute_weak__              __declspec(weak)
# define __attribute_weakref__(_symbol)  __declspec(weakref(_symbol))

#elif defined(__GNUC__)

# define GTS_DECL_EXPORT  __attribute__((visibility("default")))
# define GTS_DECL_IMPORT
# define GTS_DECL_HIDDEN  __attribute__((visibility("hidden")))

# define GTS_CXX_ATTR_USED    __attribute__((used))
# define GTS_CXX_ATTR_UNUSED  __attribute__((unused))

# define GTS_CXX_ATTR_WEAK              __attribute__((weak))
# define GTS_CXX_ATTR_WEAKREF(_symbol)  __attribute__((weakref(_symbol)))

#else // other compiler

# define GTS_DECL_EXPORT
# define GTS_DECL_IMPORT
# define GTS_DECL_HIDDEN

# define GTS_CXX_ATTR_USED
# define GTS_CXX_ATTR_UNUSED

# define GTS_CXX_ATTR_WEAK
# define GTS_CXX_ATTR_WEAKREF(_symbol)

#endif //compiler

#define C_VIRTUAL_FUNC             __attribute_weak__
#define C_VIRTUAL_SYMBOL(_symbol)  __attribute_weakref__(_symbol)

#if GTS_CPLUSPLUS >= 202002L
# define GTS_CXX_NODISCARD(_d)  [[nodiscard(_d)]]
#elif __cplusplus >= 201703L
# define GTS_CXX_NODISCARD(_d)  [[nodiscard]]
#else
# define GTS_CXX_NODISCARD(_d)
#endif


#endif //GTS_CPP_ATTRIBUTE_H
