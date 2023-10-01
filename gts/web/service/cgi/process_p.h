
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

#ifndef PROCESS_P_H
#define PROCESS_P_H

#include "process.h"
#include "gts/log.h"

#include <cppformat>
#include <iostream>
#include <cstdlib>
#include <atomic>

#ifdef _WINDOWS
# include <windows.h>
#else
# include <unistd.h>
#endif

namespace gts
{

class GTS_DECL_HIDDEN process_impl
{
	GTS_DISABLE_COPY_MOVE(process_impl)

public:
	process_impl(asio::io_context &io);
	~process_impl();

public:
	typedef std::unordered_map<std::string, std::string>  env;

public:
	asio::io_context &m_io;
	std::string m_file_name;
	std::string m_work_path;
	env m_env;

public:
#if defined(WINNT) || defined(_WINDOWS)

	HANDLE m_pwcr_fd[2] {0,0};
	HANDLE m_cwpr_fd[2] {0,0};
	std::atomic<HANDLE> m_pid;

#else //posix

	using descriptor = asio::posix::stream_descriptor;
	using descriptor_ptr = std::shared_ptr<descriptor>;

	descriptor_ptr m_write_fd;
	descriptor_ptr m_read_fd;

	int m_pwcr_fd[2] {-1,-1};
	int m_cwpr_fd[2] {-1,-1};

	std::atomic<pid_t> m_pid {-1};
	void reset_writer(pid_t pid);
	void reset_reader();
	static void signal_hander(int);

#endif //os
};

} //namespace gts

#endif //PROCESS_P_H
