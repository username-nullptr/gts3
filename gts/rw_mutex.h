
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

#include <condition_variable>
#include <atomic>
#include <mutex>

namespace gts
{

template <typename Mutex>
class GTSCORE_API basic_rw_mutex
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

private:
	std::atomic_uint m_reader {0};
	std::atomic_bool m_writer {false};
	std::condition_variable m_condition;
	Mutex m_mutex;
};

using rw_mutex = basic_rw_mutex<std::mutex>;

template <typename Mutex>
class GTS_DECL_HIDDEN basic_shared_lock
{
	GTS_DISABLE_COPY_MOVE(basic_shared_lock)

public:
	using mutex_type = Mutex;
	explicit basic_shared_lock(basic_rw_mutex<Mutex> &mutex);
	~basic_shared_lock();

private:
	basic_rw_mutex<Mutex> &m_mutex;
};

using shared_lock = basic_shared_lock<std::mutex>;

} //namespace gts

#include <gts/detail/rw_mutex.h>

#endif //GTS_RW_MUTEX_H
