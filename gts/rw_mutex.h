
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

#ifndef GTS_RW_MUTEX_H
#define GTS_RW_MUTEX_H

#include <gts/global.h>
#include <gts/exception.h>
#include <atomic>
#include <mutex>

GTS_NAMESPACE_BEGIN

template <typename Mutex>
class basic_rw_mutex
{
	GTS_DISABLE_COPY_MOVE(basic_rw_mutex)

public:
	using mutex_type = Mutex;
	basic_rw_mutex() = default;
	~basic_rw_mutex() noexcept(false);

public:
	void lock_shared();
	void lock();
	void unlock();

public:
	GTS_CXX_NODISCARD("") bool try_lock_shared() noexcept;
	GTS_CXX_NODISCARD("") bool try_lock() noexcept;

	template <class Clock, class Duration> GTS_CXX_NODISCARD("")
	bool try_lock_shared_until(const std::chrono::time_point<Clock, Duration>& atime);

	template <class Clock, class Duration> GTS_CXX_NODISCARD("")
	bool try_lock_for_until(const std::chrono::time_point<Clock, Duration>& atime);

	template <class Rep, class Period> GTS_CXX_NODISCARD("")
	bool try_lock_shared_for(const std::chrono::duration<Rep, Period>& rtime);

	template <class Rep, class Period> GTS_CXX_NODISCARD("")
	bool try_lock_for(const std::chrono::duration<Rep, Period>& rtime);

private:
	std::atomic_uint m_reader {0};
	std::atomic_bool m_writer {false};
	mutex_type m_mutex;
};

using rw_mutex = basic_rw_mutex<std::mutex>;
using timed_rw_mutex = basic_rw_mutex<std::timed_mutex>;

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Mutex>
class basic_shared_lock
{
	GTS_DISABLE_COPY(basic_shared_lock)

public:
	using mutex_type = Mutex;
	explicit basic_shared_lock(basic_rw_mutex<mutex_type> &mutex);
	basic_shared_lock(basic_rw_mutex<mutex_type> &mutex, std::try_to_lock_t);

	template <class Clock, class Duration>
	basic_shared_lock(basic_rw_mutex<mutex_type> &mutex, const std::chrono::time_point<Clock, Duration>& atime);

	template <class Rep, class Period>
	basic_shared_lock(basic_rw_mutex<mutex_type> &mutex, const std::chrono::duration<Rep, Period>& rtime);

public:
	basic_shared_lock(basic_shared_lock &&other) noexcept;
	basic_shared_lock &operator=(basic_shared_lock &&other) noexcept;
	~basic_shared_lock();

public:
	void lock_shared();
	void unlock();

public:
	GTS_CXX_NODISCARD("") bool try_lock_shared() noexcept;
	GTS_CXX_NODISCARD("") bool owns_lock() const;

	template <class Clock, class Duration> GTS_CXX_NODISCARD("")
	bool try_lock_shared_until(const std::chrono::time_point<Clock, Duration>& atime);

	template <class Rep, class Period> GTS_CXX_NODISCARD("")
	bool try_lock_shared_for(const std::chrono::duration<Rep, Period>& rtime);

public:
	void swap(basic_shared_lock& other) noexcept;
	mutex_type *release() noexcept;

private:
	basic_rw_mutex<mutex_type> *m_mutex;
	std::atomic_bool m_owns {false};
};

using shared_lock = basic_shared_lock<std::mutex>;
using timed_shared_lock = basic_shared_lock<std::timed_mutex>;

GTS_NAMESPACE_END
#include <gts/detail/rw_mutex.h>

#endif //GTS_RW_MUTEX_H
