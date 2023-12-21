#include "coro_for_asio.h"
#include "execution.h"

GTS_NAMESPACE_BEGIN

void coro_run_on_main()
{
	coro_run_on(gts_exe.io_context());
}

void coro_run_on_thread()
{
	auto context = &this_coro::get();
	context->yield([&, context]
	{
		std::thread([context]{
			context->invoke();
		})
		.detach();
	});
}

void coro_await_timer(asio::steady_timer &timer, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	timer.async_wait([&](const asio::error_code &_e)
	{
		error = _e;
		context.invoke();
	});
	context.yield();
}

GTS_NAMESPACE_END

