#ifndef GTS_DETAIL_CORO_FOR_ASIO_H
#define GTS_DETAIL_CORO_FOR_ASIO_H

GTS_NAMESPACE_BEGIN

template <typename Exe>
void coro_run_on(Exe &exe)
{
	auto context = &this_coro::get();
	context->yield([&, context]
	{
		asio::post(exe, [&, context]{
			context->invoke();
		});
	});
}

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const void *buf, std::size_t len) noexcept(false)
{
	asio::error_code error;
	auto res = coro_await_tcp_socket_write(dev, buf, len, error);
	if( error )
		throw exception("gts::coro_await_write: {}.", error);
	return res;
}

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const std::string &buf) noexcept(false)
{
	asio::error_code error;
	auto res = coro_await_tcp_socket_write(dev, buf, error);
	if( error )
		throw exception("gts::coro_await_write: {}.", error);
	return res;
}

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const void *buf, std::size_t len, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	std::size_t res = 0;

	dev.async_write_some(asio::buffer(buf, len), [&](const asio::error_code &_e, std::size_t size)
	{
		error = _e;
		res = size;
		context.invoke();
	});
	context.yield();
	return res;
}

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const std::string &buf, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	std::size_t res = 0;

	dev.async_write_some(asio::buffer(buf), [&](const asio::error_code &_e, std::size_t size)
	{
		error = _e;
		res = size;
		context.invoke();
	});
	context.yield();
	return res;
}

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Dev>
std::size_t coro_await_read(Dev &dev, void *buf, std::size_t len) noexcept(false)
{
	asio::error_code error;
	auto res = coro_await_tcp_socket_write(dev, buf, len, error);
	if( error )
		throw exception("gts::coro_await_read: {}.", error);
	return res;
}

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf, std::size_t len) noexcept(false)
{
	asio::error_code error;
	auto res = coro_await_tcp_socket_write(dev, buf, len, error);
	if( error )
		throw exception("gts::coro_await_read: {}.", error);
	return res;
}

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf) noexcept(false)
{
	asio::error_code error;
	auto res = coro_await_tcp_socket_write(dev, buf, error);
	if( error )
		throw exception("gts::coro_await_read: {}.", error);
	return res;
}

template <typename Dev>
std::size_t coro_await_read(Dev &dev, void *buf, std::size_t len, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	std::size_t res = 0;

	dev.async_read_some(asio::buffer(buf, len), [&](const asio::error_code &_e, std::size_t size)
	{
		error = _e;
		res = size;
		context.invoke();
	});
	context.yield();
	return res;
}

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf, std::size_t len, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	std::size_t res = 0;

	dev.async_read_some(asio::buffer(buf, len), [&](const asio::error_code &_e, std::size_t size)
	{
		error = _e;
		res = size;
		context.invoke();
	});
	context.yield();
	return res;
}

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf, asio::error_code &error) noexcept
{
	auto &context = this_coro::get();
	std::size_t res = 0;

	dev.async_read_some(asio::buffer(buf), [&](const asio::error_code &_e, std::size_t size)
	{
		error = _e;
		res = size;
		context.invoke();
	});
	context.yield();
	return res;
}

GTS_NAMESPACE_END


#endif //GTS_DETAIL_CORO_FOR_ASIO_H
