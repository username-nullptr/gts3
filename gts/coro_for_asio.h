#ifndef GTS_CORO_FOR_ASIO_H
#define GTS_CORO_FOR_ASIO_H

#include <asio/unyield.hpp>
#include <gts/coroutine.h>
#include <asio.hpp>

#ifdef GTS_ENABLE_SSL
# include <asio/ssl.hpp>
#endif //GTS_ENABLE_SSL

GTS_NAMESPACE_BEGIN

void coro_run_on_main();

void coro_run_on_thread();

template <typename Exe>
void coro_run_on(Exe &exe);

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const void *buf, std::size_t len) noexcept(false);

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const std::string &buf) noexcept(false);

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const void *buf, std::size_t len, asio::error_code &error) noexcept;

template <typename Dev>
std::size_t coro_await_write(Dev &dev, const std::string &buf, asio::error_code &error) noexcept;

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Dev>
std::size_t coro_await_read(Dev &dev, void *buf, std::size_t len) noexcept(false);

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf, std::size_t len) noexcept(false);

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf) noexcept(false);

template <typename Dev>
std::size_t coro_await_read(Dev &dev, void *buf, std::size_t len, asio::error_code &error) noexcept;

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf, std::size_t len, asio::error_code &error) noexcept;

template <typename Dev>
std::size_t coro_await_read(Dev &dev, std::string &buf, asio::error_code &error) noexcept;

/*--------------------------------------------------------------------------------------------------------------------*/

GTS_CXX_NODISCARD("") asio::ip::tcp::socket
coro_await_accept(asio::ip::tcp::acceptor &acceptor) noexcept(false);

GTS_CXX_NODISCARD("") asio::ip::tcp::socket
coro_await_accept(asio::ip::tcp::acceptor &acceptor, asio::error_code &error) noexcept;

void coro_await_accept
(asio::ip::tcp::acceptor &acceptor, asio::ip::tcp::socket &sock) noexcept(false);

void coro_await_accept
(asio::ip::tcp::acceptor &acceptor, asio::ip::tcp::socket &sock, asio::error_code &error) noexcept;

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef GTS_ENABLE_SSL

void coro_await_ssl_handshake(asio::ssl::stream<asio::ip::tcp::socket> &sock) noexcept(false);

void coro_await_ssl_handshake(asio::ssl::stream<asio::ip::tcp::socket> &sock, asio::error_code &error) noexcept;

#endif //GTS_ENABLE_SSL

/*--------------------------------------------------------------------------------------------------------------------*/

void coro_await_timer(asio::steady_timer &timer, asio::error_code &error) noexcept;

GTS_NAMESPACE_END
#include <gts/detail/coro_for_asio.h>


#endif //GTS_CORO_FOR_ASIO_H
