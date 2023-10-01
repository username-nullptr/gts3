
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

#ifndef GTS_DETAIL_RW_MUTEX_H
#define GTS_DETAIL_RW_MUTEX_H

namespace gts
{

template <typename Mutex>
basic_rw_mutex<Mutex>::~basic_rw_mutex() noexcept(false)
{
	if( m_writer or m_reader > 0 )
		throw exception("destruct if not unlocked.");
}

template <typename Mutex>
void basic_rw_mutex<Mutex>::lock_shared()
{
	while( m_writer )
	{
		std::unique_lock<Mutex> locker(m_mutex);
		m_condition.wait(locker);
	}
	m_reader++;
}

template <typename Mutex>
void basic_rw_mutex<Mutex>::lock()
{
	std::unique_lock<Mutex> locker(m_mutex);
	m_condition.wait(locker, [this]()->bool{
		return not m_writer and m_reader == 0;
	});
	m_writer = true;
}

template <typename Mutex>
void basic_rw_mutex<Mutex>::unlock()
{
	if( m_writer )
	{
		m_writer = false;
		m_condition.notify_all();
	}
	else if( m_reader > 0 and --m_reader == 0 )
		m_condition.notify_all();
}

template <typename Mutex>
basic_shared_lock<Mutex>::basic_shared_lock(basic_rw_mutex<Mutex> &mutex) :
	m_mutex(mutex)
{
	mutex.lock_shared();
}

template <typename Mutex>
basic_shared_lock<Mutex>::~basic_shared_lock()
{
	m_mutex.unlock();
}

} //namespace gts


#endif //GTS_DETAIL_RW_MUTEX_H
