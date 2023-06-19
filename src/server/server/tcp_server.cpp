#include "tcp_server.h"
#include "gts/gts_config_key.h"
#include "gts/algorithm.h"
#include "gts/settings.h"

#include "tcp_plugin_interface.h"
#include "application.h"
#include "app_info.h"
#include "global.h"
#include "json.h"

#include <cppfilesystem>
#include <algorithm>
#include <iostream>
#include <cctype>

namespace gts
{

namespace ip = asio::ip;

#define READ_CONFIG(_type, _key, _default) \
	gts::settings::global_instance().read<_type>(SINI_GROUP_GTS, _key, _default)

tcp_server::tcp_server() :
	m_new_connect_method(rttr::type::get_global_method(""))
#ifdef GTS_ENABLE_SSL
	,m_new_connect_method_ssl(rttr::type::get_global_method(""))
#endif //ssl
{
	auto plugin_file_name = READ_CONFIG(std::string, SINI_GTS_STRATEGY, _GTS_DEFULT_STRATEGY);
	plugin_file_name = appinfo::absolute_path(plugin_file_name);
	m_plugin_lib = std::make_shared<rttr::library>(plugin_file_name);

#ifdef GTS_ENABLE_SSL
	auto &ssl_context = asio_ssl_context();

	ssl_context.set_default_verify_paths();
	ssl_context.set_verify_mode(ssl::context::verify_client_once);

	ssl_context.set_options(ssl::context::default_workarounds |
							ssl::context::single_dh_use |
							ssl::context::no_sslv2);
	asio::error_code error;

	ssl_context.set_password_callback([](std::size_t, ssl::context::password_purpose){
		return settings::global_instance().read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_KEY);
	}, error);
	if( error )
		log_fatal("asio: ssl password failed: {}. ({})\n", error.message(), error.value());

	auto &_settings = settings::global_instance();

	auto crt_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_CRT_FILE, _GTS_SSL_CRT_DEFAULT_FILE);
	if( not crt_file.empty() )
	{
		crt_file = appinfo::absolute_path(crt_file);
		log_debug("ssl crt file: {}", crt_file);

		ssl_context.use_certificate_chain_file(crt_file, error);
		if( error )
			log_fatal("asio: ssl certificate file load failed: {}. ({})\n", error.message(), error.value());
	}

	auto key_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SSL_KEY_FILE, _GTS_SSL_KEY_DEFAULT_FILE);
	if( not key_file.empty() )
	{
		key_file = appinfo::absolute_path(key_file);
		log_debug("ssl key file: {}", key_file);

		ssl_context.use_private_key_file(key_file, ssl::context::pem, error);
		if( error )
			log_fatal("asio: ssl private key file file load failed: {}. ({})\n", error.message(), error.value());
	}

	if( not crt_file.empty() and not key_file.empty() )
		m_no_ck_file = false;
#endif //ssl
}

tcp_server::~tcp_server()
{
	stop();
}

void tcp_server::start()
{
	if( m_plugin_lib->load() == false )
		log_fatal("gts.plugin load failed: {}.\n", m_plugin_lib->get_error_string());

	auto &_settings = settings::global_instance();
	auto json_file = _settings.read<std::string>(SINI_GROUP_GTS, SINI_GTS_SITES_CONFIG);

	if( json_file.empty() )
	{
		log_warning("Sites is not configured, using default. (http:80)");
		m_sites.emplace("default", std::make_shared<tcp_site>(this, io_context(), "ipv4", 80));
		return ;
	}
	json_file = appinfo::absolute_path(json_file);

	rapidjson::Document root_object;
	auto errstr = rapidjson::from_file(json_file, root_object);

	if( not errstr.empty() )
		log_fatal("Json file read failed: {}.", errstr);

	new_connect_method_init();
	call_init();

	m_buffer_size = READ_CONFIG(int, SINI_GTS_TCP_BUF_SIZE, m_buffer_size);
	if( m_buffer_size < 1024 )
		m_buffer_size = 1024;

	for(auto it=root_object.MemberBegin(); it!=root_object.MemberEnd(); ++it)
	{
		std::string name;
		std::string addr;
		uint16_t port;

#ifdef GTS_ENABLE_SSL
		bool ssl = false;
#endif //ssl

		try {
			name = it->name.GetString();
			auto obj = it->value.GetObject();

			auto it2 = obj.FindMember("address");
			if( it2 == obj.MemberEnd() )
				throw;
			addr = to_lower(it2->value.GetString());

			it2 = obj.FindMember("port");
			if( it2 == obj.MemberEnd() )
				throw;
			port = it2->value.GetInt();

			it2 = obj.FindMember("ssl");
			if( it2 != obj.MemberEnd() )
#ifdef GTS_ENABLE_SSL
				ssl = it2->value.GetBool();
#else //no ssl
			{
				if( it2->value.GetBool() )
				{
					log_warning("Site '{}': ssl is disabled, if necessary, please recompile GTS server"
								" (cmake -DENABLE_SSL -DOpenSSL_DIR)", name);
				}
			}
#endif //ssl
		}
		catch(...) {
			log_warning("Sites config format(json) error.");
			continue;
		}

		auto sit = m_sites.find(name);
		if( sit != m_sites.end() )
		{
			log_error("Site '{}' exists.", name);
			continue;
		}

		site_ptr site;
#ifdef GTS_ENABLE_SSL
		if( ssl )
			site = std::make_shared<ssl_site>(this, io_context(), addr, port);
		else
#endif //ssl
			site = std::make_shared<tcp_site>(this, io_context(), addr, port);

		if( site->start() )
		{
			log_info("Site '{}' start ...", name);
			m_sites.emplace(std::move(name), std::move(site));
		}
		else
			log_error("Site '{}' start failed.", name);
	}
	if( m_sites.empty() )
		log_fatal("No site is available.");
}

void tcp_server::stop()
{
	if( m_sites.empty() )
		return ;

	auto method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_EXIT);
	if( method.is_valid() and method.get_parameter_infos().size() == 0 )
		method.invoke({});

	m_plugin_lib->unload();
	m_sites.clear();
}

std::string tcp_server::view_status() const
{
	std::string status = "\n";
	for(auto &pair : m_sites)
	{
		status += fmt::format("site( {} ):\n", pair.first) +
				  pair.second->view_status() +
				  "\n";
	}

	auto method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_VIEW_STATUS);
	if( method.is_valid() and method.get_return_type() == rttr::type::get<std::string>() )
		status += method.invoke({}).get_value<std::string>();
	return status;
}

void tcp_server::call_new_connect_method_0(std::shared_ptr<socket<tcp::socket>> sock)
{
	m_new_connect_method.invoke({}, std::move(*sock));
}

#ifdef GTS_ENABLE_SSL
void tcp_server::call_new_connect_method_0(std::shared_ptr<socket<ssl_stream>> sock)
{
	if( m_new_connect_method_ssl.is_valid() )
		m_new_connect_method_ssl.invoke({}, std::move(*sock));
	else
	{
		sock->shutdown(tcp::socket::shutdown_both);
		sock->close();
	}
}
#endif //ssl

#define ERROR_CHECK(_error) \
({ \
	if( error ) { \
		if( error.value() == asio::error::operation_aborted ) \
			return ; \
		else if ERR_VAL(error) \
			log_fatal("asio: accept error: {}. ({})\n", error.message(), error.value()); \
	} \
})

void tcp_server::new_connect_method_init()
{
	m_new_connect_method = rttr::type::get_global_method
						   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
								rttr::type::get<gts::socket<tcp::socket>>()
							});
	if( m_new_connect_method.is_valid() )
	{
#ifdef GTS_ENABLE_SSL
		m_new_connect_method_ssl = rttr::type::get_global_method
								   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
										rttr::type::get<gts::socket<ssl_stream>>()
									});
#endif //ssl
		m_method_id = 0;
		return ;
	}

	m_new_connect_method = rttr::type::get_global_method
						   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
								rttr::type::get<tcp::socket>(),
								rttr::type::get<void*>()
							});
	if( m_new_connect_method.is_valid() )
	{
		m_method_id = 1;
		return ;
	}

	m_new_connect_method = rttr::type::get_global_method
						   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
								rttr::type::get<tcp::socket::native_handle_type>(),
								rttr::type::get<void*>(),
								rttr::type::get<bool>()
							});
	if( m_new_connect_method.is_valid() )
	{
		m_method_id = 2;
		return ;
	}

	m_new_connect_method = rttr::type::get_global_method
						   (GTS_PLUGIN_INTERFACE_NEW_CONNECT, {
								rttr::type::get<tcp::socket::native_handle_type>(),
								rttr::type::get<void*>()
							});
	if( m_new_connect_method.is_valid() )
	{
		m_method_id = 3;
		return ;
	}
	log_fatal("gts.plugin error: strategy is null.\n");
}

void tcp_server::call_init()
{
	auto method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_INIT);
	if( method.is_valid() and method.get_parameter_infos().size() == 0 )
	{
		method.invoke({});
		return ;
	}

	method = rttr::type::get_global_method(GTS_PLUGIN_INTERFACE_INIT, {rttr::type::get<std::string>()});
	if( method.is_valid() )
		method.invoke({}, settings::global_instance().file_name());
}

/*-----------------------------------------------------------------------------------------------------------------*/

tcp_server::basic_site::basic_site(tcp_server *q_ptr, asio::io_context &io, const std::string &addr, uint16_t port) :
	q_ptr(q_ptr), m_acceptor(io), m_endpoint(tcp::v4(), 8080), m_addr(addr)
{
	asio::error_code error;
	if( addr == "localhost" )
		m_endpoint = tcp::endpoint(ip::make_address_v4("127.0.0.1"), port);

	else if( addr == "localhostipv6" or addr == "localhost_ipv6" )
		m_endpoint = tcp::endpoint(ip::make_address_v6("::1"), port);

	else if( addr == "ipv6" or addr == "anyipv6" )
		m_endpoint = tcp::endpoint(tcp::v6(), port);

	else if( addr == "ipv4" or addr == "anyipv4" or addr == "any" or addr == "auto" )
		m_endpoint = tcp::endpoint(tcp::v4(), port);

	else
	{
		m_endpoint = tcp::endpoint(ip::make_address(addr, error), port);
		if( error )
		{
			log_error("Invalid bind address.");
			m_endpoint = tcp::endpoint(tcp::v4(), port);
		}
	}
}

std::string tcp_server::basic_site::view_status() const
{
	return fmt::format("  address : {}\n"
					   "  port    : {}\n",
					   m_addr, m_endpoint.port());
}

tcp_server::basic_site::~basic_site()
{
	stop();
}

#define ACCEPTOR_OPERAT(_method, arg) \
({ \
	m_acceptor._method(arg, error); \
	if( error ) { \
		log_error("asio: socket " #_method " error: {}\n", error); \
		return false; \
	} \
})

bool tcp_server::basic_site::start()
{
	asio::error_code error;
	ACCEPTOR_OPERAT(open, m_endpoint.protocol());
	ACCEPTOR_OPERAT(set_option, tcp::acceptor::reuse_address(true));
	ACCEPTOR_OPERAT(bind, m_endpoint);
	ACCEPTOR_OPERAT(listen, asio::socket_base::max_listen_connections);
	return true;
}

void tcp_server::basic_site::stop()
{
	if( m_acceptor.is_open() )
	{
		asio::error_code error;
		m_acceptor.cancel(error);
		m_acceptor.close(error);
	}
}

std::string tcp_server::tcp_site::view_status() const
{
	return basic_site::view_status() +
			"  protocol: tcp\n";
}

bool tcp_server::tcp_site::start()
{
	if( basic_site::start() )
	{
		do_accept();
		return true;
	}
	return false;
}

void tcp_server::tcp_site::do_accept()
{
	auto sock = std::make_shared<socket<tcp::socket>>(gts_app.io_context());
	m_acceptor.async_accept(*sock, [sock, this](asio::error_code error)
	{
		ERROR_CHECK(error);
		q_ptr->service(std::move(sock));
		do_accept();
	});
}

#ifdef GTS_ENABLE_SSL

tcp_server::ssl_site::ssl_site(tcp_server *q_ptr, asio::io_context &io, const std::string &addr, uint16_t port) :
	basic_site(q_ptr, io, addr, port)
{

}

std::string tcp_server::ssl_site::view_status() const
{
	return basic_site::view_status() +
			"  protocol: ssl\n";
}

bool tcp_server::ssl_site::start()
{
	if( q_ptr->m_no_ck_file )
	{
		log_error("SSL: certificate or private-key is not configured.");
		return false;
	}
	else if( basic_site::start() )
	{
		do_accept();
		return true;
	}
	return false;
}

void tcp_server::ssl_site::do_accept()
{
	auto sock = std::make_shared<socket<ssl_stream>>(tcp::socket(io_context()), asio_ssl_context());
	m_acceptor.async_accept(sock->next_layer(), [sock, this](asio::error_code error)
	{
		ERROR_CHECK(error);
		sock->async_handshake(ssl_stream::server, [sock, this](const asio::error_code &error)
		{
			if( error )
			{
				log_warning("asio: ssl_stream handshake error: {}.", error);
				sock->close();
			}
			else
				q_ptr->service(std::move(sock));
			do_accept();
		});
	});
}

#endif //ssl

} //namespace gts
