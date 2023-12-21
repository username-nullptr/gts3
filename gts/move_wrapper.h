
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

#ifndef GTS_MOVE_WRAPPER_H
#define GTS_MOVE_WRAPPER_H

#include <gts/gts_namespace.h>
#include <gts/cpp_attribute.h>
#include <bits/move.h>

GTS_NAMESPACE_BEGIN

template <typename T>
class move_wrapper
{
public:
	move_wrapper() = default;
	explicit move_wrapper(T &&obj);

public:
	move_wrapper(const move_wrapper &other);
	move_wrapper(move_wrapper &&other) noexcept;

public:
	const T &operator*() const;
	T &operator*();

public:
	const T *operator->() const;
	T *operator->();

public:
	T &&take() const;

private:
	move_wrapper &operator=(const move_wrapper&) = default;
	move_wrapper &operator=(move_wrapper&&) noexcept = default;
	mutable T m_obj;
};

template <typename T, typename...Args>
move_wrapper<T> make_move_wrapper(Args&&...args);

template <typename T>
move_wrapper<T> make_move_wrapper(T t);

GTS_NAMESPACE_END
#include <gts/detail/move_wrapper.h>

#if GTS_CPLUSPLUS < 201402L
# define GTS_LAMBDA_MOVE(x)  x
#else //cpp11
# define GTS_LAMBDA_MOVE(x)  x = std::move(x)
#endif //cpp


#endif //GTS_MOVE_WRAPPER_H
