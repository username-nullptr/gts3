#include "socket.h"

namespace gts { namespace web
{

#ifdef GTS_ENABLE_SSL

inline tcp::endpoint socket<ssl_stream>::remote_endpoint(asio::error_code &error) {
	return this->next_layer().remote_endpoint(error);
}

inline tcp::endpoint socket<ssl_stream>::remote_endpoint() {
	return this->next_layer().remote_endpoint();
}

void socket<ssl_stream>::shutdown(tcp::socket::shutdown_type what, asio::error_code &error) {
	this->next_layer().shutdown(what, error);
}

void socket<ssl_stream>::shutdown(tcp::socket::shutdown_type what) {
	this->next_layer().shutdown(what);
}

tcp::socket::native_handle_type socket<ssl_stream>::release() {
	return this->next_layer().release();
}

tcp::socket::native_handle_type socket<ssl_stream>::release(asio::error_code &error) {
	return this->next_layer().release(error);
}

tcp::socket::native_handle_type socket<ssl_stream>::native_handle() {
	return this->next_layer().native_handle();
}

void socket<ssl_stream>::non_blocking(bool mode) {
	this->next_layer().non_blocking(mode);
}

bool socket<ssl_stream>::non_blocking() const {
	return this->next_layer().non_blocking();
}

void socket<ssl_stream>::close(asio::error_code &error) {
	this->next_layer().close(error);
}

void socket<ssl_stream>::close() {
	this->next_layer().close();
}

#endif //ssl

}} //namespace gts::web
