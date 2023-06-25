#ifndef GTS_SSL_SOCKET_H
#define GTS_SSL_SOCKET_H

#ifdef GTS_ENABLE_SSL

#include <gts/tcp_socket.h>
#include <asio/ssl.hpp>

namespace ssl = asio::ssl;

namespace gts
{

using ssl_stream = ssl::stream<tcp::socket>;

class GTSCORE_API ssl_socket : public tcp_socket
{
	GTS_DISABLE_COPY_MOVE(ssl_socket)

public:
	explicit ssl_socket(ssl_stream *sock);
	~ssl_socket() override;

public:
	std::size_t write_some(const void *buf, std::size_t size, asio::error_code &error) override;
	std::size_t read_some(std::string &buf, asio::error_code &error) override;
	std::size_t read_some(void *buf, std::size_t size, asio::error_code &error) override;

public:
	void async_write_some(const std::string &buf, std::function<void(asio::error_code, std::size_t)>) override;
	void async_write_some(const void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>) override;
	void async_read_some(std::string &buf, std::function<void(asio::error_code, std::size_t)>) override;
	void async_read_some(void *buf, std::size_t size, std::function<void(asio::error_code, std::size_t)>) override;

public:
	static ssl::context &asio_ssl_context();

protected:
	ssl_stream *m_ssl_sock;
};

} //namespace gts

#endif //ssl

#endif //GTS_SSL_SOCKET_H
