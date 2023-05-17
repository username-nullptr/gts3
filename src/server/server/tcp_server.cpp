#include "tcp_server.h"
#include "gts/algorithm.h"
#include "gts/gts_config_key.h"
#include "tcp_plugin_interface.h"
#include "app_info.h"
#include "settings.h"
#include "global.h"

#include "application.h"
#include "gts_log.h"

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
								rttr::type::get<int>()
							});

	if( m_new_connect_method.is_valid() )
		m_ncma_count = 2;
	else
	{
		m_new_connect_method = rttr::type::get_global_method
							   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
									rttr::type::get<tcp::socket::native_handle_type>()
								});

		if( m_new_connect_method.is_valid() )
			m_ncma_count = 1;
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

void tcp_server::start()
{
	auto socket = std::make_shared<tcp::socket>(gts_app.io_context());
	m_acceptor.async_accept(*socket, [socket, this](asio::error_code error)
	{
		if( error )
		{
			if( error.value() == asio::error::operation_aborted )
				return ;

			else if ERR_VAL(error)
				log_fatal("asio: accept error: {}. ({})\n", error.message(), error.value());
		}

		socket->set_option(tcp::socket::send_buffer_size(m_buffer_size), error);
		if( error )
			log_error("asio: set socket send buffer error: {}. ({})\n", error.message(), error.value());

		socket->set_option(tcp::socket::receive_buffer_size(m_buffer_size), error);
		if( error )
			log_error("asio: set socket receive buffer error: {}. ({})\n", error.message(), error.value());

		if( m_ncma_count == 1 )
			m_new_connect_method.invoke({}, socket->release());

		else if( m_ncma_count == 2 )
			m_new_connect_method.invoke({}, socket->release(), m_protocol);

		start();
	});
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

} //namespace gts
