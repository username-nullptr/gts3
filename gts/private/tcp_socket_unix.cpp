#ifdef __unix__

#include "gts/tcp_socket.h"
#include <poll.h>

namespace gts
{

static bool wait(int fd, int event, const std::chrono::milliseconds &ms, asio::error_code &error)
{
	struct pollfd fds;
	fds.fd = fd;
	fds.events = event;

	int tt = ms.count() > 0? ms.count() : -1;
	error = std::make_error_code(static_cast<std::errc>(0));

	for(;;)
	{
		int res = poll(&fds, 1, tt);
		if( res < 0 )
		{
			error = std::make_error_code(static_cast<std::errc>(errno));
			return false;
		}
		else if( res == 0 )
			return false;
		else if( res == 1 and fds.revents == event )
			break;
	}
	return true;
}

bool tcp_socket::wait_writeable(const duration &ms, asio::error_code &error)
{
	return wait(m_sock->native_handle(), POLLOUT, ms, error);
}

bool tcp_socket::wait_readable(const duration &ms, asio::error_code &error)
{
	return wait(m_sock->native_handle(), POLLIN, ms, error);
}

bool tcp_socket::wait_writeable(const duration &ms)
{
	asio::error_code error ;
	bool res = wait(m_sock->native_handle(), POLLOUT, ms, error);
	if( error )
		this->error(error, "wait_writeable");
	return res;
}

bool tcp_socket::wait_readable(const duration &ms)
{
	asio::error_code error ;
	bool res = wait(m_sock->native_handle(), POLLIN, ms, error);
	if( error )
		this->error(error, "wait_readable");
	return res;
}

} //namespace gts

#endif //__unix__
