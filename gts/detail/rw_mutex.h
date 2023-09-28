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
