#include <gts/global.h>
#include <gts/exception.h>

#include <condition_variable>
#include <atomic>
#include <mutex>

namespace gts
{

class GTSCORE_API rw_mutex
{
	GTS_DISABLE_COPY_MOVE(rw_mutex)

public:
	rw_mutex() = default;
	~rw_mutex() noexcept(false)
	{
		if( m_writer or m_reader > 0 )
			throw exception("destruct if not unlocked.");
	}

public:
	void lock_shared()
	{
		while( m_writer )
		{
			std::unique_lock<std::mutex> locker(m_mutex);
			m_condition.wait(locker);
		}
		m_reader++;
	}

	void lock()
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_condition.wait(locker, [this]()->bool{
			return not m_writer and m_reader == 0;
		});
		m_writer = true;
	}

	void unlock()
	{
		if( m_writer )
		{
			m_writer = false;
			m_condition.notify_all();
		}
		else if( m_reader > 0 and --m_reader == 0 )
			m_condition.notify_all();
	}

private:
	std::atomic_uint m_reader {0};
	std::atomic_bool m_writer {false};
	std::condition_variable m_condition;
	std::mutex m_mutex;
};

class GTS_DECL_HIDDEN shared_lock
{
public:
	explicit shared_lock(rw_mutex &mutex) : m_mutex(mutex) {
		mutex.lock_shared();
	}
	~shared_lock() {
		m_mutex.unlock();
	}

private:
	rw_mutex &m_mutex;
};

} //namespace gts
