#ifndef SOCKET_H
#define SOCKET_H

#include "global.h"

#ifdef GTS_ENABLE_SSL
# include <asio/ssl.hpp>
#endif //ssl

namespace gts { namespace web
{

template <class asio_socket>
class socket {};

template <>
class socket<tcp::socket> : public tcp::socket {
public: using tcp::socket::basic_stream_socket;
};

#ifdef GTS_ENABLE_SSL
namespace ssl = asio::ssl;
using ssl_stream = ssl::stream<tcp::socket>;

template <>
class socket<ssl_stream> : public ssl_stream
{
public:
	using ssl_stream::stream;

public:
	tcp::endpoint remote_endpoint(asio::error_code &error);
	tcp::endpoint remote_endpoint();

public:
	void shutdown(tcp::socket::shutdown_type what, asio::error_code &error);
	void shutdown(tcp::socket::shutdown_type what);

public:
	void close(asio::error_code &error);
	void close();
};

tcp::endpoint socket<ssl_stream>::remote_endpoint(asio::error_code &error) {
	return this->next_layer().remote_endpoint(error);
}

tcp::endpoint socket<ssl_stream>::remote_endpoint() {
	return this->next_layer().remote_endpoint();
}

void socket<ssl_stream>::shutdown(tcp::socket::shutdown_type what, asio::error_code &error) {
	this->next_layer().shutdown(what, error);
}

void socket<ssl_stream>::shutdown(tcp::socket::shutdown_type what) {
	this->next_layer().shutdown(what);
}

void socket<ssl_stream>::close(asio::error_code &error) {
	this->next_layer().close(error);
}

void socket<ssl_stream>::close() {
	this->next_layer().close();
}
#endif //ssl

}} //gts::web


#endif //SOCKET_H
