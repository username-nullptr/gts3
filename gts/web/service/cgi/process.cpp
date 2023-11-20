
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

#include "process_p.h"
/*
	private/process_xxx.cpp

	bool process::start(const std::string &args)
	void process::terminate()
	void process::kill()
	int process::write(const std::string &str)
	int process::read(char *buf, int size)
	bool process::is_running() const
	void process::join()
*/
GTS_NAMESPACE_BEGIN

process_impl::process_impl(asio::io_context &io) :
	m_io(io)
{

}

/*----------------------------------------------------------------------------------------------------------------------*/

process::process(asio::io_context &io, const std::string &file_name) :
	m_impl(new process_impl(io))
{
	m_impl->m_file_name = file_name;
}

process::~process()
{
	if( is_running() )
	{
		kill();
		gts_log_warning("gts::process::~process kill the process.");
	}
	delete m_impl;
}

process &process::set_file(const std::string &file_name)
{
	if( not is_running() )
		m_impl->m_file_name = file_name;
	return *this;
}

std::string process::file() const
{
	return m_impl->m_file_name;
}

process &process::add_env(const std::string &key, const std::string &value)
{
	if( is_running() )
		return *this;

	auto pair = m_impl->m_env.emplace(key, value);
	if( not pair.second )
		pair.first->second = value;
	return *this;
}

process &process::set_work_path(const std::string &path)
{
	if( not is_running() )
		m_impl->m_work_path = path;
	return *this;
}

std::size_t process::read_some(void *buf, std::size_t size, const duration &timeout, asio::error_code &error)
{
	if( wait_readable(timeout, error) )
		return read_some(buf, size, error);
	return 0;
}

std::size_t process::read_some(void *buf, std::size_t size, const duration &timeout)
{
	asio::error_code _error;
	if( wait_readable(timeout, _error) )
		return read_some(buf, size);
	else if( _error )
		error(_error, "read_some: wait_readable");
	return 0;
}

std::size_t process::read_some(void *buf, std::size_t size)
{
	asio::error_code _error;
	auto res = read_some(buf, size, _error);
	if( _error )
		error(_error, "read_some");
	return res;
}

void process::error(const asio::error_code &error, const char *func)
{
	gts_log_error("gts::process::{}: {}", func, error);
}

GTS_NAMESPACE_END
