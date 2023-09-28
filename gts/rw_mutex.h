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
