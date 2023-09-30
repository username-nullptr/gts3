#include "socket.h"

namespace gts { namespace web
{

socket::socket(tcp_socket_ptr sock) :
	m_sock(std::move(sock))
{

}

socket::~socket() noexcept
{

}

}} //namespace gts::web
