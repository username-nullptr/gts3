#ifndef PROCESS_P_H
#define PROCESS_P_H

#include "process.h"
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
#ifdef _WINDOWS

	HANDLE m_pwcr_fd[2] {0,0};
	HANDLE m_cwpr_fd[2] {0,0};
	std::atomic<HANDLE> m_pid;

#else //posix

	typedef asio::posix::stream_descriptor  descriptor;

	std::shared_ptr<descriptor> m_write_fd;
	std::shared_ptr<descriptor> m_read_fd;

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
