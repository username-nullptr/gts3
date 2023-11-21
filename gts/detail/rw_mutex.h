
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

GTS_NAMESPACE_BEGIN

template <typename Mutex>
basic_rw_mutex<Mutex>::~basic_rw_mutex() noexcept(false)
{
	if( m_writer or m_reader > 0 )
		throw exception("destruct if not unlocked.");
}

template <typename Mutex>
void basic_rw_mutex<Mutex>::lock_shared()
{
	if( m_writer )
		m_mutex.lock();
	m_reader++;
}

template <typename Mutex>
void basic_rw_mutex<Mutex>::lock()
{
	m_mutex.lock();
	m_writer = true;
}

template <typename Mutex>
void basic_rw_mutex<Mutex>::unlock()
{
	if( m_writer )
	{
		m_writer = false;
		m_mutex.unlock();
	}
	else if( m_reader > 0 and --m_reader == 0 )
		m_mutex.unlock();
}

template <typename Mutex>
bool basic_rw_mutex<Mutex>::try_lock_shared() noexcept
{
	if( m_writer )
		return false;
	m_reader++;
	return true;
}

template <typename Mutex>
bool basic_rw_mutex<Mutex>::try_lock() noexcept
{
	if( m_mutex.try_lock() )
	{
		m_writer = true;
		return true;
	}
	return false;
}

template <typename Mutex>
template <class Clock, class Duration>
bool basic_rw_mutex<Mutex>::try_lock_shared_until(const std::chrono::time_point<Clock, Duration>& atime)
{
	if( m_writer and not m_mutex.try_lock_until(atime) )
		return false;
	m_reader++;
	return true;
}

template <typename Mutex>
template <class Clock, class Duration>
bool basic_rw_mutex<Mutex>::try_lock_for_until(const std::chrono::time_point<Clock, Duration>& atime)
{
	if( m_mutex.try_lock_until(atime) )
	{
		m_writer = true;
		return true;
	}
	return false;
}

template <typename Mutex>
template <class Rep, class Period>
bool basic_rw_mutex<Mutex>::try_lock_shared_for(const std::chrono::duration<Rep, Period>& rtime)
{
	if( m_writer and not m_mutex.try_lock_for(rtime) )
		return false;
	m_reader++;
	return true;
}

template <typename Mutex>
template <class Rep, class Period>
bool basic_rw_mutex<Mutex>::try_lock_for(const std::chrono::duration<Rep, Period>& rtime)
{
	if( m_mutex.try_lock_for(rtime) )
	{
		m_writer = true;
		return true;
	}
	return false;
}

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Mutex>
basic_shared_lock<Mutex>::basic_shared_lock(basic_rw_mutex<mutex_type> &mutex) :
	m_mutex(&mutex)
{
	mutex.lock_shared();
	m_owns = true;
}

template <typename Mutex>
basic_shared_lock<Mutex>::basic_shared_lock(basic_rw_mutex<mutex_type> &mutex, std::try_to_lock_t) :
	m_mutex(&mutex)
{
	m_owns = mutex.try_lock_shared();
}

template <typename Mutex>
template <class Clock, class Duration>
basic_shared_lock<Mutex>::basic_shared_lock
(basic_rw_mutex<mutex_type> &mutex, const std::chrono::time_point<Clock, Duration>& atime) :
	m_mutex(&mutex)
{
	m_owns = mutex.try_lock_shared_until(atime);
}

template <typename Mutex>
template <class Rep, class Period>
basic_shared_lock<Mutex>::basic_shared_lock
(basic_rw_mutex<mutex_type> &mutex, const std::chrono::duration<Rep, Period>& rtime) :
	m_mutex(&mutex)
{
	m_owns = mutex.try_lock_shared_until(rtime);
}

template <typename Mutex>
basic_shared_lock<Mutex>::basic_shared_lock(basic_shared_lock &&other) noexcept :
	m_mutex(other.m_mutex), m_owns(other.m_owns)
{
	other.m_mutex = nullptr;
	other.m_owns = false;
}

template <typename Mutex>
basic_shared_lock<Mutex> &basic_shared_lock<Mutex>::operator=(basic_shared_lock &&other) noexcept
{
	if( m_owns )
		unlock();
	basic_shared_lock<Mutex>(std::move(other)).swap(*this);
	other.m_mutex = nullptr;
	other.m_owns = false;
	return *this;
}

template <typename Mutex>
basic_shared_lock<Mutex>::~basic_shared_lock()
{
	if( m_mutex )
		m_mutex->unlock();
}

template <typename Mutex>
void basic_shared_lock<Mutex>::lock_shared()
{
	if( m_mutex == nullptr )
		std::__throw_system_error(int(std::errc::operation_not_permitted));
	else if( m_owns )
		std::__throw_system_error(int(std::errc::resource_deadlock_would_occur));
	m_mutex->lock_shared();
	m_owns = true;
}

template <typename Mutex>
void basic_shared_lock<Mutex>::unlock()
{
	if( m_mutex == nullptr )
		std::__throw_system_error(int(std::errc::operation_not_permitted));
	else if( m_owns )
	{
		m_mutex->unlock();
		m_owns = false;
	}
}

template <typename Mutex>
bool basic_shared_lock<Mutex>::try_lock_shared() noexcept
{
	if( m_mutex == nullptr )
		std::__throw_system_error(int(std::errc::operation_not_permitted));
	else if( m_owns )
		std::__throw_system_error(int(std::errc::resource_deadlock_would_occur));
	m_owns = m_mutex->try_lock_shared();
	return m_owns;
}

template <typename Mutex>
bool basic_shared_lock<Mutex>::owns_lock() const
{
	return m_owns;
}

template <typename Mutex>
template <class Clock, class Duration> GTS_CXX_NODISCARD("")
bool basic_shared_lock<Mutex>::try_lock_shared_until(const std::chrono::time_point<Clock, Duration>& atime)
{
	if( m_mutex == nullptr )
		std::__throw_system_error(int(std::errc::operation_not_permitted));
	else if( m_owns )
		std::__throw_system_error(int(std::errc::resource_deadlock_would_occur));
	m_owns = m_mutex.try_lock_shared_until(atime);
	return m_owns;
}

template <typename Mutex>
template <class Rep, class Period> GTS_CXX_NODISCARD("")
bool basic_shared_lock<Mutex>::try_lock_shared_for(const std::chrono::duration<Rep, Period>& rtime)
{
	if( m_mutex == nullptr )
		std::__throw_system_error(int(std::errc::operation_not_permitted));
	else if( m_owns )
		std::__throw_system_error(int(std::errc::resource_deadlock_would_occur));
	m_owns = m_mutex.try_lock_shared_until(rtime);
	return m_owns;
}

template <typename Mutex>
void basic_shared_lock<Mutex>::swap(basic_shared_lock& other) noexcept
{
	std::swap(m_mutex, other.m_mutex);
	std::swap(m_owns, other._M_owns);
}

template <typename Mutex>
typename basic_shared_lock<Mutex>::mutex_type *basic_shared_lock<Mutex>::release() noexcept
{
	auto ret = m_mutex;
	m_mutex = nullptr;
	m_owns = false;
	return ret;
}

GTS_NAMESPACE_END


#endif //GTS_DETAIL_RW_MUTEX_H
