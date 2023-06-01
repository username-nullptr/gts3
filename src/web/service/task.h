#ifndef task_H
#define task_H

#include "service.h"
#include <functional>

namespace gts { namespace http {
struct request;
}} //namespace gts::http

namespace gts { namespace web
{

template <class asio_socket>
class GTS_DECL_HIDDEN task
{
	DISABLE_COPY(task)
	typedef gts::socket<asio_socket>              tcp_socket;
	typedef std::shared_ptr<tcp_socket>           tcp_socket_ptr;
	typedef std::shared_ptr<const http::request>  http_request_ptr;

public:
	explicit task(tcp_socket_ptr socket);
	void start(http_request_ptr request);

public:
	void async_wait_next(std::function<void()> call_back);
	void cancel();

private:
	void run();

private:
	void GET    (service_io<asio_socket> &sio);
	void POST   (service_io<asio_socket> &sio);
	void PUT    (service_io<asio_socket> &sio);
	void HEAD   (service_io<asio_socket> &sio);
	void DELETE (service_io<asio_socket> &sio);
	void OPTIONS(service_io<asio_socket> &sio);
	void CONNECT(service_io<asio_socket> &sio);
	void TRACH  (service_io<asio_socket> &sio);

private:
	tcp_socket_ptr m_socket;
	http_request_ptr m_request;
	std::function<void()> m_call_back;
};

class GTS_DECL_HIDDEN scheduler
{
public:
	static asio::thread_pool *pool;
	static std::string cgi_path;

public:
	static void init();
	static void exit();
};

template <class asio_socket>
task<asio_socket>::task(tcp_socket_ptr socket) :
	m_socket(std::move(socket))
{

}

template <class asio_socket>
void task<asio_socket>::start(http_request_ptr request)
{
	m_request = std::move(request);
	m_socket->non_blocking(false);

	asio::post(*scheduler::pool, [this]
	{
		run();
		io_context().post([this]
		{
			if( m_call_back )
			{
				m_request.reset();
				m_call_back();
			}
		});
	});
}

template <class asio_socket>
void task<asio_socket>::async_wait_next(std::function<void()> call_back)
{
	assert(call_back != nullptr);
	m_call_back = std::move(call_back);
}

template <class asio_socket>
void task<asio_socket>::cancel()
{
	m_call_back = nullptr;
}

template <class asio_socket>
void task<asio_socket>::run()
{
	auto method = m_request->method;
	log_debug() << "URL:" << m_request->path << method;

	service_io<asio_socket> sio(*m_socket, *m_request);

	if(      method == "GET"     ) GET    (sio);
	else if( method == "POST"    ) POST   (sio);
	else if( method == "HEAD"    ) HEAD   (sio);
	else if( method == "PUT"     ) PUT    (sio);
	else if( method == "DELETE"  ) DELETE (sio);
	else if( method == "OPTIONS" ) OPTIONS(sio);
	else if( method == "CONNECT" ) CONNECT(sio);
	else if( method == "TRACH"   ) TRACH  (sio);

	if( not m_request->keep_alive )
	{
		m_socket->shutdown(tcp::socket::shutdown_both);
		m_socket->close();
	}
}

#define _TASK_DO_PARSING(_sio) \
({ \
	sio.url_name = _sio.request.path; \
	if( sio.url_name.empty() ) \
		sio.url_name = "/index.html"; \
	else { \
		auto pos = sio.url_name.find("/", 1); \
		if( pos != std::string::npos ) { \
			auto prefix = sio.url_name.substr(0,pos); \
			if( prefix == "/cgi-bin" ) { \
				sio.url_name = scheduler::cgi_path + sio.url_name.substr(pos+1); \
				return service::call_cgi_service(sio); \
			} else if ( prefix == "/plugin-lib" ) { \
				sio.url_name.erase(0, pos+1); \
				return service::call_plugin_service(sio); \
			} \
		} \
	} \
	if( fs::is_directory(sio.url_name) ) \
		return _sio.return_to_null(http::hs_not_found); \
	sio.url_name; \
})

template <class asio_socket>
void task<asio_socket>::GET(service_io<asio_socket> &sio)
{
	sio.url_name = service_io_config::resource_path + _TASK_DO_PARSING(sio);
	service::call_static_resource_service(sio);
}

template <class asio_socket>
void task<asio_socket>::POST(service_io<asio_socket> &sio)
{
	_TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

template <class asio_socket>
void task<asio_socket>::PUT(service_io<asio_socket> &sio)
{
	_TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

template <class asio_socket>
void task<asio_socket>::HEAD(service_io<asio_socket> &sio)
{
	sio.url_name = sio.request.path;
	if( sio.url_name == "/" )
		return sio.return_to_null();

	auto pos = sio.url_name.find("/", 1);

	if( pos != std::string::npos )
	{
		auto prefix = sio.url_name.substr(0,pos);
		if( prefix == "/cgi-bin" )
		{
			if( fs::exists(scheduler::cgi_path + sio.url_name.substr(pos+1)) )
				return sio.return_to_null();
			return sio.return_to_null(http::hs_not_found);
		}
		else if ( prefix == "/plugin-lib" )
		{
			sio.url_name.erase(0, pos+1);
			return service::call_plugin_service(sio);
		}
	}

	else if( fs::exists(service_io_config::resource_path + sio.url_name) )
		sio.return_to_null(http::hs_not_found);
	else
		sio.return_to_null();
}

template <class asio_socket>
void task<asio_socket>::DELETE(service_io<asio_socket> &sio)
{
	_TASK_DO_PARSING(sio);
	sio.return_to_null(http::hs_not_implemented);
}

template <class asio_socket>
void task<asio_socket>::OPTIONS(service_io<asio_socket> &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

template <class asio_socket>
void task<asio_socket>::CONNECT(service_io<asio_socket> &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

template <class asio_socket>
void task<asio_socket>::TRACH(service_io<asio_socket> &sio) {
	sio.return_to_null(http::hs_service_unavailable);
}

}} //namespace gts::web


#endif //task_H
