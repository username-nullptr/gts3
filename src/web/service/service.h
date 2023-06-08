#include "service/static_resource_service.h"
#include "service/plugins_service.h"
#include "service/cgi_service.h"

namespace gts { namespace web
{

class GTS_DECL_HIDDEN service
{
public:
	template <class asio_socket>
	static void call_static_resource_service(service_io<asio_socket> &sio);

	template <class asio_socket>
	static void call_plugin_service(service_io<asio_socket> &sio);

	template <class asio_socket>
	static void call_cgi_service(service_io<asio_socket> &sio);

public:
	static void init(const basic_string_list &args);
	static void exit();
};

template <class asio_socket>
void service::call_static_resource_service(service_io<asio_socket> &sio) {
	static_resource_service<asio_socket>(sio).call();
}

template <class asio_socket>
void service::call_plugin_service(service_io<asio_socket> &sio) {
	plugin_service<asio_socket>(sio).call();
}

template <class asio_socket>
void service::call_cgi_service(service_io<asio_socket> &sio) {
	cgi_service<asio_socket>(sio).call();
}

}} //namespace gts::web
