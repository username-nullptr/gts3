#include "tcp_server.h"
#include "gts/algorithm.h"
#include "gts/gts_config_key.h"
#include "tcp_plugin_interface.h"
#include "application.h"
#include "app_info.h"
#include "settings.h"
#include "global.h"

#include <algorithm>
#include <iostream>
#include <cctype>

namespace gts
{

namespace ip = asio::ip;

#define ACCEPTOR_OPERAT(_method, arg) \
({ \
	m_acceptor._method(arg, error); \
	if( error ) \
		log_fatal("asio: socket " #_method " error: {}. ({})\n", error.message(), error.value()); \
})

#define READ_CONFIG(_type, _key, _default) \
	gts::settings::global_instance().read<_type>(SINI_GROUP_GTS, _key, _default)

tcp_server::tcp_server(int, const char**) :
	m_acceptor(gts_app.io_context()),
	m_new_connect_method(rttr::type::get_global_method(""))
{
	asio::error_code error;
	tcp::endpoint endpoint(tcp::v4(), 8080);

	m_port = READ_CONFIG(uint16_t, SINI_GTS_PORT, 8080);
	auto addr = to_lower(READ_CONFIG(std::string, SINI_GTS_ADDRESS, "any"));

	if( addr == "localhost" )
	{
		endpoint = tcp::endpoint(ip::make_address_v4("127.0.0.1"), m_port);
		m_protocol = 4;
	}

	else if( addr == "localhostipv6" )
	{
		endpoint = tcp::endpoint(ip::make_address_v6("::1"), m_port);
		m_protocol = 6;
	}

	else if( addr == "ipv6" or addr == "anyipv6" )
	{
		endpoint = tcp::endpoint(tcp::v6(), m_port);
		m_protocol = 6;
	}

	else if( addr == "ipv4" or addr == "anyipv4" or addr == "any" or addr == "auto" )
	{
		endpoint = tcp::endpoint(tcp::v4(), m_port);
		m_protocol = 4;
	}
	else
	{
		endpoint = tcp::endpoint(ip::make_address(addr, error), m_port);
		if( error )
		{
			log_error("Invalid bind address.");
			endpoint = tcp::endpoint(tcp::v4(), m_port);
			m_protocol = 4;
		}
	}

	ACCEPTOR_OPERAT(open, endpoint.protocol());
	ACCEPTOR_OPERAT(set_option, tcp::acceptor::reuse_address(true));
	ACCEPTOR_OPERAT(bind, endpoint);
	ACCEPTOR_OPERAT(listen, asio::socket_base::max_listen_connections);

	auto plugin_file_name = READ_CONFIG(std::string, SINI_GTS_STRATEGY, _GTS_DEFULT_STRATEGY);
	if( not starts_with(plugin_file_name, "/") )
		plugin_file_name = appinfo::dir_path() + plugin_file_name;

	m_plugin_lib = std::make_shared<rttr::library>(plugin_file_name);
	if( m_plugin_lib->load() == false )
		log_fatal("gts.plugin load failed: {}.\n", m_plugin_lib->get_error_string());

	auto method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_INIT, {rttr::type::get<std::string>()});
	if( method.is_valid() )
		method.invoke({}, settings::global_instance().file_name());
	else
	{
		method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_INIT);
		if( method.is_valid() )
			method.invoke({});
	}

	m_new_connect_method = rttr::type::get_global_method
						   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
								rttr::type::get<tcp::socket::native_handle_type>(),
								rttr::type::get<bool>(),
								rttr::type::get<int>()
							});

	if( m_new_connect_method.is_valid() )
		m_ncma_count = 3;
	else
	{
		m_new_connect_method = rttr::type::get_global_method
							   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
									rttr::type::get<tcp::socket::native_handle_type>(),
									rttr::type::get<bool>()
								});

		if( m_new_connect_method.is_valid() )
			m_ncma_count = 2;
		else
			log_fatal("gts.plugin error: strategy is null.\n");
	}

	m_buffer_size = READ_CONFIG(int, SINI_GTS_TCP_BUF_SIZE, m_buffer_size);
	if( m_buffer_size < 1024 )
		m_buffer_size = 1024;
}

tcp_server::~tcp_server()
{
	stop();
}

#ifdef GTS_ENABLE_SSL
static ssl::context *g_ssl_context = nullptr;
#endif //ssl

void tcp_server::start()
{
#ifdef GTS_ENABLE_SSL
	auto &_settings = settings::global_instance();
	if( _settings.read<bool>(SINI_GROUP_GTS, SINI_GTS_ENABLE_SSL) == false )
		return start_tcp();

	log_info("gts: enable ssl.");
	g_ssl_context = new ssl::context(ssl::context::sslv23);

	g_ssl_context->set_options(ssl::context::default_workarounds |
							   ssl::context::single_dh_use |
							   ssl::context::no_sslv2);
	asio::error_code error;

	g_ssl_context->set_password_callback([](std::size_t, ssl::context::password_purpose){
		return std::string("seri");
	}, error);
	if( error )
		log_fatal("asio: ssl password failed: {}. ({})\n", error.message(), error.value());

	g_ssl_context->use_certificate_chain_file("/root/.ssl/server.crt", error);
	if( error )
		log_fatal("asio: ssl certificate file load failed: {}. ({})\n", error.message(), error.value());

	g_ssl_context->use_private_key_file("/root/.ssl/server.key", ssl::context::pem, error);
	if( error )
		log_fatal("asio: ssl private file file load failed: {}. ({})\n", error.message(), error.value());

	start_ssl();
#else //no ssl
	start_tcp();
#endif //ssl
}

void tcp_server::stop()
{
	if( not m_acceptor.is_open() )
		return ;

	auto method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_EXIT);
	if( method.is_valid() )
		method.invoke({});

	m_plugin_lib->unload();

	asio::error_code error;
	m_acceptor.cancel(error);
	m_acceptor.close(error);
}

uint16_t tcp_server::port() const
{
	return m_port;
}

std::string tcp_server::view_status() const
{
	auto status = fmt::format("port: {}\n", port());
	auto method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_VIEW_STATUS);

	if( method.is_valid() )
	{
		if( method.get_return_type() == rttr::type::get<std::string>() )
			status += method.invoke({}).get_value<std::string>();
	}
	return status;
}

#define ERROR_CHECK(_error) \
({ \
	if( error ) { \
		if( error.value() == asio::error::operation_aborted ) \
			return ; \
		else if ERR_VAL(error) \
			log_fatal("asio: accept error: {}. ({})\n", error.message(), error.value()); \
	} \
})

void tcp_server::start_tcp()
{
	auto _socket = std::make_shared<socket<tcp::socket>>(gts_app.io_context());
	m_acceptor.async_accept(*_socket, [_socket, this](asio::error_code error)
	{
		ERROR_CHECK(error);
		service(std::move(_socket));
		start_tcp();
	});
}

#ifdef GTS_ENABLE_SSL
void tcp_server::start_ssl()
{
	auto _socket = std::make_shared<socket<ssl_stream>>(tcp::socket(gts_app.io_context()), *g_ssl_context);
	m_acceptor.async_accept(_socket->next_layer(), [_socket, this](asio::error_code error)
	{
		ERROR_CHECK(error);
		_socket->async_handshake(ssl_stream::server, [_socket, this](const asio::error_code &error)
		{
			if( error and error.value() != 336151574 )
			{
				log_error("asio: ssl_stream handshake error: {}. ({})", error.message(), error.value());
				_socket->close();
			}
			else
				service(std::move(_socket));
			start_ssl();
		});
	});
}
#endif //ssl

} //namespace gts
