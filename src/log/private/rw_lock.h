#include "gts/gts_global.h"
#include <condition_variable>
#include <atomic>
#include <mutex>

namespace gts { namespace log
{

class GTS_DECL_HIDDEN rw_mutex
{
public:
	rw_mutex() = default;
	~rw_mutex() noexcept(false);

public:
	void lock_shared();
	void lock();
	void unlock();

private:
	std::atomic_uint m_reader;
	std::atomic_bool m_writer;
	std::condition_variable m_condition;
	std::mutex m_mutex;
};

class GTS_DECL_HIDDEN shared_lock
{
public:
	explicit shared_lock(rw_mutex &mutex);
	~shared_lock();

private:
	rw_mutex &m_mutex;
};

}} //namespace gts::log
