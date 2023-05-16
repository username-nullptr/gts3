#include "rw_lock.h"

namespace gts { namespace log
{

rw_mutex::~rw_mutex() noexcept(false)
{
	if( m_writer or m_reader > 0 )
		throw "destruct if not unlocked.";
}

void rw_mutex::lock_shared()
{
	while( m_writer )
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_condition.wait(locker);
	}
	m_reader++;
}

void rw_mutex::lock()
{
	while( m_writer or m_reader > 0 )
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_condition.wait(locker);
	}
	m_writer = true;
}

void rw_mutex::unlock()
{
	if( m_writer )
	{
		m_writer = false;
		m_condition.notify_all();
	}
	else if( m_reader > 0 and --m_reader == 0 )
		m_condition.notify_all();
}

shared_lock::shared_lock(rw_mutex &mutex) :
	m_mutex(mutex)
{
	mutex.lock_shared();
}

shared_lock::~shared_lock()
{
	m_mutex.unlock();
}

}} //namespace gts::log
