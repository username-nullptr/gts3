
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

#ifndef GTS_DETAIL_MOVE_WRAPPER_H
#define GTS_DETAIL_MOVE_WRAPPER_H

GTS_NAMESPACE_BEGIN

template <typename T>
move_wrapper<T>::move_wrapper(T &&obj) :
	m_obj(std::move(obj))
{

}

template <typename T>
move_wrapper<T>::move_wrapper(const move_wrapper &other) :
	m_obj(std::move(other.m_obj))
{

}

template <typename T>
move_wrapper<T>::move_wrapper(move_wrapper &&other) noexcept :
	m_obj(std::move(other.m_obj))
{

}

template <typename T>
const T &move_wrapper<T>::operator*() const
{
	return m_obj;
}

template <typename T>
T &move_wrapper<T>::operator*()
{
	return m_obj;
}

template <typename T>
const T *move_wrapper<T>::operator->() const
{
	return &m_obj;
}

template <typename T>
T *move_wrapper<T>::operator->()
{
	return &m_obj;
}

template <typename T>
T &&move_wrapper<T>::take() const
{
	return std::move(m_obj);
}

template <typename T, typename...Args>
move_wrapper<T> make_move_wrapper(Args&&...args)
{
	return move_wrapper<T>(T(std::forward<Args>(args)...));
}

template <typename T>
move_wrapper<T> make_move_wrapper(T t)
{
	return move_wrapper<T>(std::move(t));
}

GTS_NAMESPACE_END


#endif //GTS_DETAIL_MOVE_WRAPPER_H
