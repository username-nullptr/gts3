#include "coro_for_asio.h"
#include "execution.h"

using namespace asio::ip;

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

tcp::socket coro_await_accept(tcp::acceptor &acceptor) noexcept(false)
{
	asio::error_code error;
	auto sock = coro_await_accept(acceptor, error);
	if( error )
		throw exception("gts::coro_await_accept: {}.", error);
	return sock;
}

tcp::socket coro_await_accept(tcp::acceptor &acceptor, asio::error_code &error) noexcept
{
	tcp::socket sock(gts_exe.io_context());
	acceptor.async_accept(sock, [&](asio::error_code _e){
		error = _e;
	});
	return sock;
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

