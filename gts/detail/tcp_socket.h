#ifndef GTS_DETAIL_TCP_SOCKET_H
#define GTS_DETAIL_TCP_SOCKET_H

namespace gts
{

template <typename SettableSocketOption>
void tcp_socket::set_option(const SettableSocketOption &option, asio::error_code &error) noexcept
{
	m_sock->set_option(option, error);
}

template <typename SettableSocketOption>
void tcp_socket::set_option(const SettableSocketOption &option) noexcept
{
	asio::error_code error;
	m_sock->set_option(option, error);
	if( error )
		this->error(error, "set_option");
}

template <typename GettableSocketOption>
void tcp_socket::get_option(GettableSocketOption &option, asio::error_code &error) const  noexcept
{
	m_sock->get_option(option, error);
}

template <typename GettableSocketOption>
void tcp_socket::get_option(GettableSocketOption &option) const noexcept
{
	asio::error_code error;
	m_sock->get_option(option, error);
	if( error )
		this->error(error, "get_option");
}

} //namespace gts


#endif //GTS_DETAIL_TCP_SOCKET_H
