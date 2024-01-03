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
	auto &context = this_coro::get();
	context.yield([&]
	{
		std::thread([&]{
			if( not context.is_finished() )
				context.invoke();
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
	coro_await_accept(acceptor, sock, error);
	return sock;
}

void coro_await_accept
(asio::ip::tcp::acceptor &acceptor, asio::ip::tcp::socket &sock) noexcept(false)
{
	asio::error_code error;
	coro_await_accept(acceptor, sock, error);
	if( error )
		throw exception("gts::coro_await_accept: {}.", error);
}

void coro_await_accept
(asio::ip::tcp::acceptor &acceptor, asio::ip::tcp::socket &sock, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	acceptor.async_accept(sock, [&](asio::error_code _e)
	{
		error = _e;
		context.invoke();
	});
	context.yield();
}

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef GTS_ENABLE_SSL

using ssl_stream = asio::ssl::stream<tcp::socket>;

void coro_await_ssl_handshake(ssl_stream &sock) noexcept(false)
{
	asio::error_code error;
	coro_await_ssl_handshake(sock, error);
	if( error )
		throw exception("gts::coro_await_handshake: {}.", error);
}

void coro_await_ssl_handshake(ssl_stream &sock, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	sock.async_handshake(ssl_stream::server, [&](const asio::error_code &_e)
	{
		error = _e;
		context.invoke();
	});
	context.yield();
}

#endif //GTS_ENABLE_SSL

/*--------------------------------------------------------------------------------------------------------------------*/

void coro_await_timer
(asio::steady_timer &timer, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	timer.async_wait([&](const asio::error_code &_e)
	{
		error = _e;
		context.invoke();
	});
	context.yield();
}

void coro_await_timer
(asio::steady_timer &timer, const asio::steady_timer::duration &time, asio::error_code &error) noexcept
{
	timer.expires_after(time);
	coro_await_timer(timer, error);
}

GTS_NAMESPACE_END

