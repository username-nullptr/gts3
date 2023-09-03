#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <thread>
#include <rttr/type>
#include "gts/log.h"

#ifdef GTS_ENABLE_SSL
# include "gts/ssl_socket.h"
#else
# include "gts/tcp_socket.h"
#endif

namespace gts
{

class tcp_server
{
	GTS_DISABLE_COPY_MOVE(tcp_server)

public:
	tcp_server();
	~tcp_server();

public:
	void start();
	void stop();

public:
	std::string view_status() const;

private:
	void service(tcp_socket *sock, bool universal);

private:
	class basic_site
	{
		GTS_DISABLE_COPY_MOVE(basic_site)

	public:
		basic_site(tcp_server *q_ptr, asio::io_context &io, const site_info &info);
		virtual ~basic_site() = 0;

	public:
		virtual std::string view_status() const;
		virtual bool start();
		void stop();

	protected:
		tcp_server *q_ptr;
		tcp::acceptor m_acceptor;
		tcp::endpoint m_endpoint;
		std::string m_addr;
		bool m_universal;
	};

	class tcp_site : public basic_site
	{
	public:
		using basic_site::basic_site;
		std::string view_status() const override;
		bool start() override;

	private:
		void do_accept();
	};

#ifdef GTS_ENABLE_SSL
	class ssl_site : public basic_site
	{
	public:
		using basic_site::basic_site;
		std::string view_status() const override;
		bool start() override;

	private:
		void do_accept();
	};
	bool m_no_ck_file = true;
#endif //ssl

private:
	typedef std::shared_ptr<basic_site>  site_ptr;
	std::map<std::string, site_ptr> m_sites;
	int m_buffer_size = 65536;
};

} //namespace gts


#endif //TCP_SERVER_H
