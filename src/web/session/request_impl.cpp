#include "request_impl.h"
#include "gts/algorithm.h"

namespace gts { namespace http
{

std::size_t request_impl::tcp_ip_buffer_size() const
{
	tcp::socket::send_buffer_size attr;
	m_socket->get_option(attr);
	return attr.value();
}

void request_impl::finish()
{
	auto it = m_headers.find("connection");
	if( it == m_headers.end() )
		m_keep_alive = m_version != "1.0";
	else
		m_keep_alive = str_to_lower(it->second) != "close";

	it = m_headers.find("accept-encoding");
	if( it == m_headers.end() )
		m_support_gzip = false;
	else
	{
		for(auto &str : str_split(it->second, ","))
		{
			if( str_to_lower(str_trimmed(str)) == "gzip" )
			{
				m_support_gzip = true;
				break;
			}
		}
	}
}

}} //namespace gts::http
