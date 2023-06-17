#ifdef __unix__

#include "interaction_p.h"
#include "application.h"
#include "app_info.h"
#include "global.h"
#include "gts/log.h"

#include <unordered_map>
#include <cppfilesystem>
#include <asio.hpp>
#include <cassert>
#include <memory>
#include <cstdio>
#include <set>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace gts { namespace cmdline
{

class GTS_DECL_HIDDEN pipe_ope
{
public:
	explicit pipe_ope(interaction_private *q_ptr);
	~pipe_ope();

public:
	bool open();
	void close();

public:
	bool is_open() const;

public:
	int read(char *buf, int len, int timeout);
	int write(const char *buf, int len, int timeout);

public:
	void async_read(char *buf, int len, std::function<void(int len)> call_back);
	void async_write(char *buf, int len, std::function<void(int len)> call_back);
	void cancel();

private:
	interaction_private *q_ptr;
	bool m_reading = false;

	std::string m_file_names[2];
	int m_pipe[2] {-1,-1};

	typedef asio::posix::stream_descriptor  descriptor;

	descriptor *m_write_pipe = nullptr;
	descriptor *m_read_pipe = nullptr;
};

pipe_ope::pipe_ope(interaction_private *q_ptr) :
	q_ptr(q_ptr)
{
	auto dir_name = appinfo::tmp_dir_path() + "cmdline/";

	if( not fs::exists(dir_name) )
		fs::create_directories(dir_name);

	auto file_name = dir_name + "default";

	m_file_names[0] = file_name + ".write";
	m_file_names[1] = file_name + ".read";
}

pipe_ope::~pipe_ope()
{
	close();
}

bool pipe_ope::open()
{
	if( is_open() )
		return true;

	for(int i=0; i<2; i++)
	{
		if( not fs::exists(m_file_names[i]) and mkfifo(m_file_names[i].c_str(), 0644) < 0 )
		{
			log_error("pipe[0] '{}' create failed: {}.", m_file_names[i], strerror(errno));
			return false;
		}
	}

	m_pipe[0] = ::open(m_file_names[0].c_str(), O_RDWR);
	if( m_pipe[0] < 0 )
	{
		log_error("pipe '{}' open failed: {}.", m_file_names[0], strerror(errno));
		return false;
	}

	m_pipe[1] = ::open(m_file_names[1].c_str(), O_RDWR);
	if( m_pipe[1] < 0 )
	{
		log_error("pipe '{}' open failed: {}.", m_file_names[1], strerror(errno));
		::close(m_pipe[0]);
		return false;
	}

	else if( q_ptr->m_is_server )
	{
		m_write_pipe = new descriptor(gts::io_context(), m_pipe[1]);
		m_read_pipe = new descriptor(gts::io_context(), m_pipe[0]);
	}
	else
	{
		m_write_pipe = new descriptor(gts::io_context(), m_pipe[0]);
		m_read_pipe = new descriptor(gts::io_context(), m_pipe[1]);
	}
	return true;
}

void pipe_ope::close()
{
	if( not is_open() )
		return ;

	cancel();
	if( m_reading )
	{
		std::error_code error;
		m_read_pipe->non_blocking(false, error);
		m_read_pipe->write_some(asio::buffer("__c", 3), error);
	}

	delete m_write_pipe; m_write_pipe = nullptr;
	delete m_read_pipe; m_read_pipe = nullptr;

	m_pipe[0] = -1;
	m_pipe[1] = -1;

	if( q_ptr->m_is_server )
		fs::remove_all(appinfo::tmp_dir_path() + "cmdline/");
}

bool pipe_ope::is_open() const
{
	return m_pipe[0] != -1;
}

int pipe_ope::read(char *buf, int len, int timeout)
{
	GTS_UNUSED(timeout);
	if( not is_open() )
	{
		log_error("pipe not open.");
		return -1;
	}

	assert(buf);
	if( len < 0 )
		return -1;
	else if( len == 0 )
		return 0;

	m_reading = true;
	m_read_pipe->non_blocking(true);

	struct pollfd fds;
	fds.events = POLLIN;
	fds.fd = m_read_pipe->native_handle();
	int res = poll(&fds, 1, timeout);

	if( res == 0 )
		return 0;
	else if( res < 0 )
	{
		if( errno == EINTR )
			return 0;

		log_error("read: poll error: {}", strerror(errno));
		return res;
	}

	res = ::read(fds.fd, buf, len);
	m_reading = false;

	if( res < 0 )
	{
		log_error("read: {}.", strerror(errno));
		return -errno;
	}
	else if( buf[0] == '_' and buf[1] == '_' and buf[2] == 'c' )
	{
		buf[0] = '\0';
		return 0;
	}
	return res;
}

int pipe_ope::write(const char *buf, int len, int timeout)
{
	GTS_UNUSED(timeout);
	if( not is_open() )
	{
		log_error("pipe not open.");
		return -1;
	}

	assert(buf);
	if( len < 0 )
		return -1;
	else if( len == 0 )
		return 0;

	m_write_pipe->non_blocking(true);

	struct pollfd fds;
	fds.events = POLLOUT;
	fds.fd = m_write_pipe->native_handle();
	int res = poll(&fds, 1, timeout);

	if( res == 0 )
		return 0;
	else if( res < 0 )
	{
		if( errno == EINTR )
			return 0;

		log_error("write: poll error: {}", strerror(errno));
		return res;
	}

	res = ::write(fds.fd, buf, len);
	if( res < len )
		log_error("write: {}.", strerror(errno));
	return res;
}

void pipe_ope::async_read(char *buf, int len, std::function<void(int)> call_back)
{
	if( not is_open() )
	{
		log_error("pipe not open.");
		gts::io_context().post([call_back]{
			call_back(0);
		});
		return ;
	}

	assert(buf);
	if( len < 0 )
		return call_back(-1);
	else if( len == 0 )
		return call_back(0);

	m_read_pipe->async_read_some
			(asio::buffer(buf, len), [call_back](const asio::error_code &error, std::size_t size)
	{
		if( error )
		{
			if( error.value() == asio::error::operation_aborted )
				call_back(0);
			else
			{
				log_error("read: {}.", error.value());
				call_back(-1);
			}
		}
		else
			call_back(size);
	});
}

void pipe_ope::async_write(char *buf, int len, std::function<void(int)> call_back)
{
	if( not is_open() )
	{
		log_error("pipe not open.");
		gts::io_context().post([call_back]{
			call_back(0);
		});
		return ;
	}

	assert(buf);
	if( len < 0 )
		return call_back(-1);
	else if( len == 0 )
		return call_back(0);

	m_write_pipe->async_write_some
			(asio::buffer(buf, len), [call_back](const asio::error_code &error, std::size_t size)
	{
		if( error )
		{
			if( error.value() == asio::error::operation_aborted )
				call_back(0);
			else
			{
				log_error("write: {}.", error.value());
				call_back(-1);
			}
		}
		else
			call_back(size);
	});
}

void pipe_ope::cancel()
{
	asio::error_code error;
	m_read_pipe->cancel(error);
	m_write_pipe->cancel(error);
}

/*---------------------------------------------------------------------------------------*/

interaction_private::interaction_private(bool is_server) :
	m_is_server(is_server),
	m_ope(new pipe_ope(this))
{

}

interaction_private::~interaction_private()
{
	delete m_ope;
}

/*---------------------------------------------------------------------------------------*/

bool interaction::open()
{
	return d_ptr->m_ope->open();
}

void interaction::close()
{
	d_ptr->m_ope->close();
}

bool interaction::is_open() const
{
	return d_ptr->m_ope->is_open();
}

int interaction::read(char *buf, int len, int timeout)
{
	return d_ptr->m_ope->read(buf, len, timeout);
}

int interaction::write(const char *buf, int len, int timeout)
{
	return d_ptr->m_ope->write(buf, len, timeout);
}

void interaction::async_read(char *buf, int len, std::function<void(int)> call_back)
{
	d_ptr->m_ope->async_read(buf, len, std::move(call_back));
}

void interaction::async_write(char *buf, int len, std::function<void(int)> call_back)
{
	d_ptr->m_ope->async_write(buf, len, std::move(call_back));
}

void interaction::cancel()
{
	d_ptr->m_ope->cancel();
}

}} //namespace gts::cmdline

#endif //__unix__
