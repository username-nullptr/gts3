#ifndef GTS_SOCKET_H
#define GTS_SOCKET_H

#include <gts/gts_global.h>
#ifdef USING_ASIO

#ifdef GTS_ENABLE_SSL
# include <asio/ssl.hpp>
namespace ssl = asio::ssl;
#endif //ssl

using namespace asio::ip;

namespace gts
{

template <class asio_socket>
class socket {};

#ifdef GTS_ENABLE_SSL
using ssl_stream = ssl::stream<tcp::socket>;
#endif //ssl

template <>
class socket<tcp::socket> : public tcp::socket
{
public:
	using tcp::socket::socket;
	socket(tcp::socket &&other);

#ifdef GTS_ENABLE_SSL
	ssl_stream::native_handle_type ssl() { return nullptr; }
	ssl_stream::native_handle_type release_ssl() { return nullptr; }
#else
	void *ssl() { return nullptr; }
	void *release_ssl() { return nullptr; }
#endif //ssl

public:
	const tcp::socket &next_layer() const;
	tcp::socket &next_layer();
};

#ifdef GTS_ENABLE_SSL
GTSCORE_API ssl::context &asio_ssl_context();

template <>
class socket<ssl_stream> : public ssl_stream
{
public:
	using ssl_stream::stream;

public:
	tcp::endpoint remote_endpoint(asio::error_code &error);
	tcp::endpoint remote_endpoint();

public:
	tcp::endpoint local_endpoint(asio::error_code &error);
	tcp::endpoint local_endpoint();

public:
	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption& option, asio::error_code &error);

	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption& option);

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption& option, asio::error_code &error) const;

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption& option) const;

public:
	void shutdown(tcp::socket::shutdown_type what, asio::error_code &error);
	void shutdown(tcp::socket::shutdown_type what);

public:
	tcp::socket::native_handle_type release();
	tcp::socket::native_handle_type release(asio::error_code &error);
	tcp::socket::native_handle_type native_handle();

public:
	ssl_stream::native_handle_type release_ssl();
	ssl_stream::native_handle_type ssl();

public:
	void non_blocking(bool mode, asio::error_code& error);
	void non_blocking(bool mode);
	bool non_blocking() const;

public:
	bool is_open() const;
	void close(asio::error_code &error);
	void close();
};

template <typename SettableSocketOption>
void socket<ssl_stream>::set_option(const SettableSocketOption& option, asio::error_code &error) {
	this->next_layer().set_option(option, error);
}

template <typename SettableSocketOption>
void socket<ssl_stream>::set_option(const SettableSocketOption& option) {
	this->next_layer().set_option(option);
}

template <typename GettableSocketOption>
void socket<ssl_stream>::get_option(GettableSocketOption& option, asio::error_code &error) const {
	this->next_layer().get_option(option, error);
}

template <typename GettableSocketOption>
void socket<ssl_stream>::get_option(GettableSocketOption& option) const {
	this->next_layer().get_option(option);
}
#endif //ssl

} //namespace gts

#endif //using asio

#endif //GTS_SOCKET_H
