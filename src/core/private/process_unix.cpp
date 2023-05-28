#ifdef __unix__

#include "process_p.h"
#include <cassert>
#include <mutex>
#include <map>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace gts
{

static std::map<pid_t, process*> g_process_map;
//static std::mutex g_mutex;

process_private::~process_private()
{
	if( m_pid > 0 )
		g_process_map.erase(m_pid);
}

void process_private::reset_writer(pid_t pid)
{
	g_process_map.erase(pid);
	m_read_fd->cancel();

	m_write_fd->cancel();
	m_write_fd.reset();

	close(m_pwcr_fd[1]);
}

void process_private::reset_reader()
{
	m_read_fd->cancel();
	m_read_fd.reset();
	close(m_cwpr_fd[0]);
}

/*---------------------------------------------------------------------------------------------------------------*/

bool process::start(const string_list &args)
{
	if( d_ptr->m_pid > 0 )
		return true;

	else if( d_ptr->m_file_name.empty() )
	{
		std::cerr << "***Error: process::start: file name is empty." << std::endl;
		return false;
	}

	if( d_ptr->m_read_fd )
		d_ptr->reset_reader();

	if( pipe2(d_ptr->m_cwpr_fd, O_NONBLOCK) < 0 )
	{
		std::cerr << "***Error: process::start: pipe: read fd open failed: " << strerror(errno) << std::endl;
		return false;
	}

	else if( pipe2(d_ptr->m_pwcr_fd, O_NONBLOCK) < 0 )
	{
		std::cerr << "***Error: process::start: pipe: write fd open failed: " << strerror(errno) << std::endl;
		close(d_ptr->m_cwpr_fd[0]);
		close(d_ptr->m_cwpr_fd[1]);
		return false;
	}

	d_ptr->m_pid = vfork();
	if( d_ptr->m_pid < 0 )
	{
		std::cerr << "***Error: process::start: fork failed: " << strerror(errno) << std::endl;
		return false;
	}

	else if( d_ptr->m_pid == 0 ) //child
	{
		fcntl(d_ptr->m_cwpr_fd[1], F_SETFL, fcntl(d_ptr->m_cwpr_fd[1], F_GETFL) & ~O_NONBLOCK);
		dup2(d_ptr->m_pwcr_fd[0], STDIN_FILENO);
		dup2(d_ptr->m_cwpr_fd[1], STDOUT_FILENO);

		int res = chdir(d_ptr->m_work_path.c_str());
		_UNUSED(res);

		for(auto &env : d_ptr->m_env)
			setenv(env.first.c_str(), env.second.c_str(), 1);

		const char **_args = new const char*[args.size() + 2] {d_ptr->m_file_name.c_str(), nullptr};
		for(std::size_t i=0; i<args.size(); i++)
		{
			if( not args[i].empty() )
				_args[i+1] = args[i].c_str();
		}

		res = execvp(d_ptr->m_file_name.c_str(), const_cast<char**>(_args));
		delete[] _args;

		perror("***Error: execvp");
		_exit(res);
	}

	d_ptr->m_read_fd = std::make_shared<process_private::descriptor>(d_ptr->m_io, d_ptr->m_cwpr_fd[0]);
	d_ptr->m_write_fd = std::make_shared<process_private::descriptor>(d_ptr->m_io, d_ptr->m_pwcr_fd[1]);

//	g_mutex.lock();
	g_process_map.emplace(d_ptr->m_pid, this);
//	g_mutex.unlock();
	return true;
}

void process::terminate()
{
	if( d_ptr->m_pid > 0 )
		::kill(d_ptr->m_pid, SIGTERM);
}

void process::kill()
{
	if( d_ptr->m_pid > 0 )
		::kill(d_ptr->m_pid, SIGKILL);
}

int process::write(const char *buf, int size, int timeout)
{
	_UNUSED(buf);
	_UNUSED(size);
	_UNUSED(timeout);
	return -1;
}

int process::read(const char *buf, int size, int timeout)
{
	_UNUSED(buf);
	_UNUSED(size);
	_UNUSED(timeout);
	return -1;
}

void process::async_write(const char *buf, int size, std::function<void(asio::error_code,std::size_t)> call_back)
{
	if( d_ptr->m_pid > 0 )
	{
		d_ptr->m_write_fd->non_blocking(true);
		d_ptr->m_write_fd->async_write_some(asio::buffer(buf, size), std::move(call_back));
	}
	else
	{
		d_ptr->m_io.post([call_back]{
			call_back(std::make_error_code(std::errc::operation_canceled), 0);
		});
	}
}

void process::async_read(char *buf, int size, std::function<void(asio::error_code,std::size_t)> call_back)
{
	// fuck ?!?
	if( d_ptr->m_read_fd == nullptr )
	{
		d_ptr->m_io.post([call_back]{
			call_back(std::make_error_code(std::errc::operation_canceled), 0);
		});
		return ;
	}

	d_ptr->m_read_fd->non_blocking(true);

	int res = ::read(d_ptr->m_read_fd->native_handle(), buf, size);
	if( res > 0 )
	{
		d_ptr->m_io.post([call_back, res]{
			call_back({}, static_cast<std::size_t>(res));
		});
		return ;
	}
	else if( res == 0 )
	{
		if( d_ptr->m_pid < 0 )
		{
			d_ptr->m_io.post([call_back]{
				call_back(std::make_error_code(std::errc::operation_canceled), 0);
			});
			return d_ptr->reset_reader();
		}
	}
	else if( errno != EAGAIN and errno != EWOULDBLOCK )
	{
		d_ptr->m_io.post([call_back]{
			call_back(std::make_error_code(static_cast<std::errc>(errno)), 0);
		});
		if( d_ptr->m_pid < 0 )
			d_ptr->reset_reader();
		return ;
	}

	// If the peer is closed, the callback function should theoretically be fired immediately
	// ith size equal to 0, but in practice the callback function is never called.
	d_ptr->m_read_fd->async_read_some
			(asio::buffer(buf, size),
			 [this, call_back](const asio::error_code &error, std::size_t size)
	{
		call_back(error, size);
		if( size == 0 )
			d_ptr->reset_reader();
	});
}

void process::cancel()
{
	if( d_ptr->m_pid <= 0 )
		return ;

	d_ptr->m_write_fd->cancel();
	d_ptr->m_read_fd->cancel();
}

#if 0
int process::write(char *buf, int size)
{
	if( d_ptr->m_pid < 0 or size == 0 )
		return 0;

	int res = ::write(d_ptr->m_pwcr_fd[1], buf, size);
	if( res > 0 )
		return res;

	std::cerr << "***Error: process::write error: " << strerror(errno) << std::endl;
	return res;
}

int process::read(char *buf, int size)
{
	assert(buf);
	if( d_ptr->m_pid < 0 or size == 0 )
		return 0;

	int res = ::read(d_ptr->m_cwpr_fd[0], buf, size);
	if( res > 0 )
		return res;

	std::cerr << "***Error: process::read error: " << strerror(errno) << std::endl;
	return res;
}
#endif //01

bool process::is_running() const
{
	return d_ptr->m_pid > 0;
}

void process::join()
{
	if( d_ptr->m_pid <= 0 )
		return ;

	pid_t pid = waitpid(d_ptr->m_pid, nullptr, 0);
	d_ptr->m_pid = -1;

	if( pid > 0 )
		d_ptr->reset_writer(pid);
}

/*---------------------------------------------------------------------------------------------------------------*/

static __sighandler_t pre_signal_hander = signal(SIGCHLD, &process_private::signal_hander);

void process_private::signal_hander(int signo)
{
	if( signo == SIGCHLD )
	{
		pid_t pid = waitpid(-1, nullptr, WNOHANG);

//		g_mutex.lock();
		auto it = g_process_map.find(pid);
		if( it != g_process_map.end() )
		{
			it->second->d_ptr->m_pid = -1;
			it->second->d_ptr->reset_writer(pid);
		}
//		g_mutex.unlock();
	}

	else if( pre_signal_hander )
		pre_signal_hander(signo);
}

} //namespace gts

#endif //__unix__