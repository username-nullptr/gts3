#ifndef SOCKET_H
#define SOCKET_H

#include "gts/gts_global.h"
#include <asio.hpp>

#ifdef GTS_ENABLE_SSL
# include <asio/ssl.hpp>
namespace ssl = asio::ssl;
#endif //ssl

using namespace asio::ip;

namespace gts
{

template <class asio_socket>
class socket {};

template <>
class socket<tcp::socket> : public tcp::socket
{
public:
	using tcp::socket::basic_stream_socket;
	typedef std::false_type  is_ssl;
};

#ifdef GTS_ENABLE_SSL
using ssl_stream = ssl::stream<tcp::socket>;

template <>
class socket<ssl_stream> : public ssl_stream
{
public:
	using ssl_stream::stream;
	typedef std::true_type  is_ssl;

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


#endif //SOCKET_H
