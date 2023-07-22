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

class GTS_DECL_HIDDEN session_impl
{
	GTS_DISABLE_COPY_MOVE(session_impl)

public:
	explicit session_impl(uint64_t s)
	{
		if( s == 0 )
			m_lifecycle = s;
		else
			m_lifecycle = g_global_lifecycle;
	}

public:
	uint64_t m_lifecycle;
	asio::steady_timer m_timer { io_context() };

	std::string m_id { uuid::generate() };
	uint64_t m_create_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

	session_attributes m_attributes;
	bool m_is_valid = true;
};

/*------------------------------------------------------------------------------------------------------------*/

session::session(uint64_t s) :
	m_impl(new session_impl(s))
{
	g_rw_mutex.lock();
	g_session_hash.emplace(id(), session_ptr(this));
	g_rw_mutex.unlock();
	set_lifecycle(m_impl->m_lifecycle);
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

const session_attributes &session::attributes() const
{
	return m_impl->m_attributes;
}

session_attributes &session::attributes()
{
	return m_impl->m_attributes;
}

const rttr::variant &session::attribute(const std::string &key) const
{
	auto it = m_impl->m_attributes.find(key);
	if( it == m_impl->m_attributes.end() )
		throw exception("gts::http::session::attribute: 'key' does not exist.");
	return it->second;
}

rttr::variant session::attribute_or(const std::string &key, const rttr::variant &deft_value) const
{
	auto it = m_impl->m_attributes.find(key);
	return it == m_impl->m_attributes.end()? deft_value : it->second;
}

rttr::variant session::attribute_or(const std::string &key, rttr::variant &&deft_value) const
{
	auto it = m_impl->m_attributes.find(key);
	return it == m_impl->m_attributes.end()? std::move(deft_value) : it->second;
}

session &session::set_attribute(const std::string &key, const rttr::variant &value)
{
	auto res = m_impl->m_attributes.emplace(key, value);
	if( res.second == false and res.first != m_impl->m_attributes.end() )
		res.first->second = value;
	return *this;
}

session &session::set_attribute(const std::string &key, rttr::variant &&value)
{
	auto res = m_impl->m_attributes.emplace(key, std::move(value));
	if( res.second == false and res.first != m_impl->m_attributes.end() )
		res.first->second = std::move(value);
	return *this;
}

session &session::unset_attribute(const std::string &key)
{
	m_impl->m_attributes.erase(key);
	return *this;
}

uint64_t session::lifecycle() const
{
	return m_impl->m_lifecycle;
}

session &session::set_lifecycle(uint64_t s)
{
	if( s == 0 )
		m_impl->m_lifecycle = s;
	else
		m_impl->m_lifecycle = g_global_lifecycle;

	m_impl->m_timer.cancel();
	m_impl->m_timer.expires_after(seconds(m_impl->m_lifecycle));

	m_impl->m_timer.async_wait([this](const asio::error_code &was_cencel)
	{
		if( was_cencel )
		{
			if( m_impl->m_is_valid )
				return ;
		}
		else
		{
			m_impl->m_is_valid = false;
			m_impl->m_attributes.clear();
		}

		g_rw_mutex.lock();
		g_session_hash.erase(id());
		g_rw_mutex.unlock();
	});
	return *this;
}

void session::invalidate()
{
	m_impl->m_is_valid = false;
	m_impl->m_attributes.clear();
	m_impl->m_timer.cancel();
}

std::shared_ptr<session> session::make_shared(uint64_t seconds)
{
	return std::make_shared<session>(seconds);
}

session_ptr session::get(const std::string &id)
{
	shared_lock locker(g_rw_mutex); GTS_UNUSED(locker);
	auto it = g_session_hash.find(id);
	return it == g_session_hash.end()? session_ptr() : it->second;
}

void session::set_global_lifecycle(uint64_t seconds)
{
	if( seconds > 0 )
		g_global_lifecycle = seconds;
}

uint64_t session::global_lifecycle()
{
	return g_global_lifecycle;
}

}} //namespace gts::http
