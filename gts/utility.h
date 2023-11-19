
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

#ifndef GTS_UTILITY_H
#define GTS_UTILITY_H

#include <rttr/variant.h>
#include <gts/cplusplus.hpp>

#ifdef __GNUC__
# include <cxxabi.h>
# define _ABI_CXA_DEMANGLE(name)  abi::__cxa_demangle(name, 0,0,0)
#else //_MSVC
# define _ABI_CXA_DEMANGLE(name)  name
#endif //compiler

#define GTS_UNUSED(x)  (void)(x)

#if defined(_WIN64) || defined(__x86_64__) || defined(__arm64__) || defined(__aarch64__)
# define GTS_OS_64BIT
#else
# define _OS_32BIT
#endif // 32bit & 64bit

#define GTS_DISABLE_COPY(_class) \
	explicit _class(const _class&) = delete; \
	void operator=(const _class&) = delete;

#define GTS_DISABLE_MOVE(_class) \
	explicit _class(_class&&) = delete; \
	void operator=(_class&&) = delete;

#define GTS_DISABLE_COPY_MOVE(_class) \
	GTS_DISABLE_COPY(_class) GTS_DISABLE_MOVE(_class)

#define GTS_NAMESPACE_BEGIN  namespace gts {
#define GTS_NAMESPACE_END    } //namespace gts

GTS_NAMESPACE_BEGIN

template <typename T>
inline const char *type_name(T &&t) {
	return _ABI_CXA_DEMANGLE(typeid(t).name());
}

template <typename T>
inline const char *type_name() {
	return _ABI_CXA_DEMANGLE(typeid(T).name());
}

template <bool C, typename T = void>
using enable_if_t = typename std::enable_if<C,T>::type;

template <typename T>
using decay_t = typename std::decay<T>::type;

#define gts_is_arithmetic(...)  std::is_arithmetic<__VA_ARGS__>::value

#define gts_is_enum(...)   std::is_enum<__VA_ARGS__>::value

#define gts_is_same(...)   std::is_same<__VA_ARGS__>::value

#define gts_is_dsame(x,y)   std::is_same<gts::decay_t<x>, y>::value

#define gts_is_base_of(...)   std::is_base_of<__VA_ARGS__>::value

GTS_NAMESPACE_END

#define GTS_DECLVAL(...)   std::declval<__VA_ARGS__>()

#define GTS_CLASS_METHOD_DECLVAL(Class, Return, ...)   (GTS_DECLVAL(Class).*GTS_DECLVAL(Return(Class::*)(__VA_ARGS__)))

#define GTS_TYPE_DECLTYPE(...)   typename _GTD_0 = decltype(__VA_ARGS__)

#define GTS_TYPE_ENABLE_IF(...)   typename _GTEI_0 = gts::enable_if_t<__VA_ARGS__>

#define GTS_RTTR_TYPE(...)   rttr::type::get<__VA_ARGS__>()

#define GTS_RTTR_TYPE_BY_NAME(...)   rttr::type::get_by_name(__VA_ARGS__)


#endif //GTS_UTILITY_H
