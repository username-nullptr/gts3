#ifndef SOCKET_H
#define SOCKET_H

#include "global.h"

#ifdef GTS_ENABLE_SSL
# include <asio/ssl.hpp>
#endif //ssl

namespace gts { namespace web
{

template <class asio_socket>
class socket {
	enum { is_ssl = false };
};

template <>
class socket<tcp::socket> : public tcp::socket
{
public:
	using tcp::socket::basic_stream_socket;
	enum { is_ssl = false };
};

#ifdef GTS_ENABLE_SSL
namespace ssl = asio::ssl;
using ssl_stream = ssl::stream<tcp::socket>;

template <>
class socket<ssl_stream> : public ssl_stream
{
public:
	using ssl_stream::stream;
	enum { is_ssl = true };

public:
	tcp::endpoint remote_endpoint(asio::error_code &error);
	tcp::endpoint remote_endpoint();

public:
	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption& option);

	template <typename SettableSocketOption>
	void set_option(const SettableSocketOption& option, asio::error_code &error);

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption& option) const;

	template <typename GettableSocketOption>
	void get_option(GettableSocketOption& option, asio::error_code &error) const;

public:
	void shutdown(tcp::socket::shutdown_type what, asio::error_code &error);
	void shutdown(tcp::socket::shutdown_type what);

public:
	tcp::socket::native_handle_type release();
	tcp::socket::native_handle_type release(asio::error_code &error);
	tcp::socket::native_handle_type native_handle();

public:
	void non_blocking(bool mode);
	bool non_blocking() const;

public:
	void close(asio::error_code &error);
	void close();
};

template <typename SettableSocketOption>
void socket<ssl_stream>::set_option(const SettableSocketOption& option) {
	this->next_layer().set_option(option);
}

template <typename SettableSocketOption>
void socket<ssl_stream>::set_option(const SettableSocketOption& option, asio::error_code &error) {
	this->next_layer().set_option(option, error);
}

template <typename GettableSocketOption>
void socket<ssl_stream>::get_option(GettableSocketOption& option) const {
	this->next_layer().get_option(option);
}

template <typename GettableSocketOption>
void socket<ssl_stream>::get_option(GettableSocketOption& option, asio::error_code &error) const {
	this->next_layer().get_option(option, error);
}
#endif //ssl

}} //namespace gts::web


#endif //SOCKET_H
