
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

#include "session.h"
#include "gts/uuid.h"
#include "gts/rw_mutex.h"
#include <chrono>

using namespace std::chrono;

namespace gts { namespace http
{

static std::atomic<uint64_t> g_global_lifecycle {1800}; //s

static std::unordered_map<std::string, session_ptr> g_session_hash;

static rw_mutex g_rw_mutex;

using duration = std::chrono::seconds;

using unique_lock = std::unique_lock<rw_mutex>;

class GTS_DECL_HIDDEN session_impl
{
	GTS_DISABLE_COPY_MOVE(session_impl)

public:
	explicit session_impl(const duration &s) {
		set_lifecycle(s);
	}

public:
	void set_lifecycle(const duration &s)
	{
		if( s.count() > 0 )
			m_lifecycle = s.count();
		else
			m_lifecycle = g_global_lifecycle.load();
	}

public:
	void restart(uint64_t s = 0)
	{
		m_timer.cancel();

		if( s == 0 )
			m_timer.expires_after(seconds(m_lifecycle));
		else
			m_timer.expires_after(seconds(s));

		m_timer.async_wait([this](const asio::error_code &was_cencel)
		{
			if( was_cencel )
			{
				if( m_is_valid )
					return ;
			}
			else
			{
				m_is_valid = false;
				m_attrs_mutex.lock();
				m_attributes.clear();
				m_attrs_mutex.unlock();
			}

			g_rw_mutex.lock();
			g_session_hash.erase(m_id);
			g_rw_mutex.unlock();
		});
	}

public:
	std::atomic<uint64_t> m_lifecycle;
	asio::steady_timer m_timer { io_context() };

	std::string m_id { uuid::generate() };
	uint64_t m_create_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

	rw_mutex m_attrs_mutex;
	session_attributes m_attributes;
	std::atomic_bool m_is_valid { true };
};

/*------------------------------------------------------------------------------------------------------------*/

session::session(const duration &s) :
	m_impl(new session_impl(s))
{

}

session::~session()
{
	delete m_impl;
}

std::string session::id() const
{
	return m_impl->m_id;
}

uint64_t session::create_time() const
{
	return m_impl->m_create_time;
}

bool session::is_valid() const
{
	return m_impl->m_is_valid;
}

rttr::variant session::attribute(const std::string &key) const noexcept(false)
{
	shared_lock locker(m_impl->m_attrs_mutex); GTS_UNUSED(locker);
	auto it = m_impl->m_attributes.find(key);
	if( it == m_impl->m_attributes.end() )
		throw exception("gts::http::session::attribute: key '{}' does not exist.", key);
	return it->second;
}

rttr::variant session::attribute_or(const std::string &key, rttr::variant deft_value) const noexcept
{
	shared_lock locker(m_impl->m_attrs_mutex); GTS_UNUSED(locker);
	auto it = m_impl->m_attributes.find(key);
	return it == m_impl->m_attributes.end()? std::move(deft_value) : it->second;
}

session &session::set_attribute(std::string key, rttr::variant value)
{
	m_impl->m_attrs_mutex.lock();
	m_impl->m_attributes[std::move(key)] = std::move(value);
	m_impl->m_attrs_mutex.unlock();
	return *this;
}

session &session::unset_attribute(const std::string &key)
{
	m_impl->m_attrs_mutex.lock();
	m_impl->m_attributes.erase(key);
	m_impl->m_attrs_mutex.unlock();
	return *this;
}

std::size_t session::attribute_count() const
{
	shared_lock locker(m_impl->m_attrs_mutex); GTS_UNUSED(locker);
	return m_impl->m_attributes.size();
}

session_attributes session::attributes() const
{
	shared_lock locker(m_impl->m_attrs_mutex); GTS_UNUSED(locker);
	return m_impl->m_attributes;
}

string_list session::attribute_key_list() const
{
	string_list list;
	m_impl->m_attrs_mutex.lock_shared();

	for(auto &pair : m_impl->m_attributes)
		list.emplace_back(pair.first);

	m_impl->m_attrs_mutex.unlock();
	return list;
}

std::set<std::string> session::attribute_key_set() const
{
	std::set<std::string> set;
	m_impl->m_attrs_mutex.lock_shared();

	for(auto &pair : m_impl->m_attributes)
		set.emplace(pair.first);

	m_impl->m_attrs_mutex.unlock();
	return set;
}

uint64_t session::lifecycle() const
{
	return m_impl->m_lifecycle;
}

session &session::set_lifecycle(const duration &s)
{
	m_impl->set_lifecycle(s);
	uint64_t ctime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
	uint64_t cat = m_impl->m_create_time + s.count();

	if( ctime < cat )
		m_impl->restart(ctime - cat);
	else
		invalidate();
	return *this;
}

session &session::expand(const duration &s)
{
	if( s.count() > 0 )
		m_impl->m_lifecycle = s.count();
	m_impl->restart();
	return *this;
}

void session::invalidate()
{
	m_impl->m_is_valid = false;
	m_impl->m_attrs_mutex.lock();
	m_impl->m_attributes.clear();
	m_impl->m_attrs_mutex.unlock();
	m_impl->m_timer.cancel();
}

session_ptr session::make_shared(const duration &s)
{
	auto obj = new session(s); set(obj);
	return session::get(obj->id());
}

session_ptr session::get(const std::string &id)
{
	shared_lock locker(g_rw_mutex); GTS_UNUSED(locker);
	auto it = g_session_hash.find(id);
	return it == g_session_hash.end()? session_ptr() : it->second;
}

void session::set(session *obj)
{
	if( obj == nullptr )
		return ;

	g_rw_mutex.lock();
	g_session_hash.emplace(obj->id(), session_ptr(obj));

	g_rw_mutex.unlock();
	obj->m_impl->restart();
}

void session::set_global_lifecycle(const duration &seconds)
{
	if( seconds.count() > 0 )
		g_global_lifecycle = seconds.count();
}

duration session::global_lifecycle()
{
	return duration(g_global_lifecycle);
}

}} //namespace gts::http
