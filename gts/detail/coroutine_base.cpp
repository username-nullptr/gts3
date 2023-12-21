#include "coroutine_base.h"

GTS_CORO_DETAIL_NAMESPACE_BEGIN

coroutine_base::~coroutine_base() = default;

void coroutine_base::set_finished_callback(std::function<void()> func)
{
	m_finished_callback = std::move(func);
}

void coroutine_base::set_again(bool flag)
{
	if( not is_finished() )
		m_again = flag;
}

bool coroutine_base::is_finished() const noexcept
{
	return m_coro->is_finished();
}

void coroutine_base::invoke() noexcept(false)
{
	do {
		m_again = false;
		this_coro::_set(this);

		auto _r = m_coro->start();
		if( _r != 0 )
			throw exception("coroutine_base::invoke: coroutine start failed ({}).", _r);

		else if( m_yield_func )
		{
			m_yield_func();
			m_yield_func = {};
		}
	}
	while( m_again );
}

void coroutine_base::yield() noexcept(false)
{
	auto _r = m_coro->yield();
	if( _r != 0 )
		throw exception("coroutine_base::yield: coroutine yield failed ({}).", _r);
}

void coroutine_base::_finished()
{
	this_coro::_set(nullptr);
	if( m_finished_callback )
		m_finished_callback();
}

/*--------------------------------------------------------------------------------------------------------------------*/

this_coro::~this_coro() = default;

thread_local coroutine_base *g_context = nullptr;

inline coro_context &this_coro::get() noexcept(false)
{
	if( g_context == nullptr )
		throw exception("coroutine_context::get: This is not the coroutine context.");
	return *g_context;
}

void this_coro::invoke()
{
	get().invoke();
}

void this_coro::operator()()
{
	get().invoke();
}

void this_coro::yield()
{
	get().yield();
}

void this_coro::_set(coroutine_base *c)
{
	g_context = c;
}

GTS_CORO_DETAIL_NAMESPACE_END
