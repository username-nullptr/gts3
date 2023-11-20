
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifdef __unix__

#include "interaction_p.h"
#include "gts/private/app_info.h"
#include "gts/log.h"

#include <cppfilesystem>
#include <asio.hpp>
#include <cassert>
#include <set>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

GTS_CMDLINE_NAMESPACE_BEGIN

class GTS_DECL_HIDDEN pipe_ope
{
public:
	explicit pipe_ope(interaction_private *q_ptr);
	~pipe_ope();

public:
	bool open();
	void close();

public:
	GTS_CXX_NODISCARD("")
	bool is_open() const;

public:
	ssize_t read(char *buf, ssize_t len, int timeout);
	ssize_t write(const char *buf, ssize_t len, int timeout);

public:
	void async_read(char *buf, ssize_t len, std::function<void(ssize_t)> call_back);
	void async_write(char *buf, ssize_t len, std::function<void(ssize_t)> call_back);
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
	auto dir_name = app::tmp_dir_path() + "cmdline/";

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
	for(int i : {0,1})
	{
		if( not fs::exists(m_file_names[i]) and mkfifo(m_file_names[i].c_str(), 0644) < 0 )
		{
			gts_log_error("pipe[0] '{}' create failed: {}.", m_file_names[i], strerror(errno));
			return false;
		}
	}
	m_pipe[0] = ::open(m_file_names[0].c_str(), O_RDWR);
	if( m_pipe[0] < 0 )
	{
		gts_log_error("pipe '{}' open failed: {}.", m_file_names[0], strerror(errno));
		return false;
	}
	m_pipe[1] = ::open(m_file_names[1].c_str(), O_RDWR);
	if( m_pipe[1] < 0 )
	{
		gts_log_error("pipe '{}' open failed: {}.", m_file_names[1], strerror(errno));
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
	delete m_write_pipe;
	m_write_pipe = nullptr;

	delete m_read_pipe;
	m_read_pipe = nullptr;

	m_pipe[0] = -1;
	m_pipe[1] = -1;

	if( q_ptr->m_is_server )
		fs::remove_all(app::tmp_dir_path() + "cmdline/");
}

bool pipe_ope::is_open() const
{
	return m_pipe[0] != -1;
}

ssize_t pipe_ope::read(char *buf, ssize_t len, int timeout)
{
	GTS_UNUSED(timeout);
	if( not is_open() )
	{
		gts_log_error("pipe not open.");
		return -1;
	}
	assert(buf);
	if( len < 0 )
		return -1;
	else if( len == 0 )
		return 0;

	m_reading = true;
	m_read_pipe->non_blocking(true);

	struct pollfd fds {0};
	fds.events = POLLIN;
	fds.fd = m_read_pipe->native_handle();
	long res = poll(&fds, 1, timeout);

	if( res == 0 )
		return 0;
	else if( res < 0 )
	{
		if( errno == EINTR )
			return 0;

		gts_log_error("read: poll error: {}", strerror(errno));
		return res;
	}
	res = ::read(fds.fd, buf, len);
	m_reading = false;

	if( res < 0 )
	{
		gts_log_error("read: {}.", strerror(errno));
		return -errno;
	}
	else if( buf[0] == '_' and buf[1] == '_' and buf[2] == 'c' )
	{
		buf[0] = '\0';
		return 0;
	}
	return res;
}

ssize_t pipe_ope::write(const char *buf, ssize_t len, int timeout)
{
	GTS_UNUSED(timeout);
	if( not is_open() )
	{
		gts_log_error("pipe not open.");
		return -1;
	}
	assert(buf);
	if( len < 0 )
		return -1;
	else if( len == 0 )
		return 0;
	m_write_pipe->non_blocking(true);

	struct pollfd fds {0};
	fds.events = POLLOUT;
	fds.fd = m_write_pipe->native_handle();
	long res = poll(&fds, 1, timeout);

	if( res == 0 )
		return 0;
	else if( res < 0 )
	{
		if( errno == EINTR )
			return 0;

		gts_log_error("write: poll error: {}", strerror(errno));
		return res;
	}
	res = ::write(fds.fd, buf, len);
	if( res < len )
		gts_log_error("write: {}.", strerror(errno));
	return res;
}

void pipe_ope::async_read(char *buf, ssize_t len, std::function<void(ssize_t)> call_back)
{
	if( not is_open() )
	{
		gts_log_error("pipe not open.");
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

	m_read_pipe->async_read_some(asio::buffer(buf, len),
	[call_back](const asio::error_code &error, std::size_t size)
	{
		if( error )
		{
			if( error.value() == asio::error::operation_aborted )
				call_back(0);
			else
			{
				gts_log_error("read: {}.", error.value());
				call_back(-1);
			}
		}
		else
			call_back(static_cast<ssize_t>(size));
	});
}

void pipe_ope::async_write(char *buf, ssize_t len, std::function<void(ssize_t)> call_back)
{
	if( not is_open() )
	{
		gts_log_error("pipe not open.");
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

	m_write_pipe->async_write_some(asio::buffer(buf, len),
	[call_back](const asio::error_code &error, std::size_t size)
	{
		if( error )
		{
			if( error.value() == asio::error::operation_aborted )
				call_back(0);
			else
			{
				gts_log_error("write: {}.", error.value());
				call_back(-1);
			}
		}
		else
			call_back(static_cast<ssize_t>(size));
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

ssize_t interaction::read(char *buf, ssize_t len, int timeout)
{
	return d_ptr->m_ope->read(buf, len, timeout);
}

ssize_t interaction::write(const char *buf, ssize_t len, int timeout)
{
	return d_ptr->m_ope->write(buf, len, timeout);
}

void interaction::async_read(char *buf, ssize_t len, std::function<void(ssize_t)> call_back)
{
	d_ptr->m_ope->async_read(buf, len, std::move(call_back));
}

void interaction::async_write(char *buf, ssize_t len, std::function<void(ssize_t)> call_back)
{
	d_ptr->m_ope->async_write(buf, len, std::move(call_back));
}

void interaction::cancel()
{
	d_ptr->m_ope->cancel();
}

GTS_CMDLINE_NAMESPACE_END

#endif //__unix__
