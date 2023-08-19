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

process_impl::~process_impl()
{
	if( m_pid > 0 )
		g_process_map.erase(m_pid);
}

void process_impl::reset_writer(pid_t pid)
{
	g_process_map.erase(pid);
	m_read_fd->cancel();

	m_write_fd->cancel();
	m_write_fd.reset();

	close(m_pwcr_fd[1]);
}

void process_impl::reset_reader()
{
	m_read_fd->cancel();
	m_read_fd.reset();
	close(m_cwpr_fd[0]);
}

/*---------------------------------------------------------------------------------------------------------------*/

bool process::start(const string_list &args)
{
	if( m_impl->m_pid > 0 )
		return true;

	else if( m_impl->m_file_name.empty() )
	{
		std::cerr << "***Error: process::start: file name is empty." << std::endl;
		return false;
	}

	if( m_impl->m_read_fd )
		m_impl->reset_reader();

	if( pipe2(m_impl->m_cwpr_fd, O_NONBLOCK) < 0 )
	{
		std::cerr << "***Error: process::start: pipe: read fd open failed: " << strerror(errno) << std::endl;
		return false;
	}

	else if( pipe2(m_impl->m_pwcr_fd, O_NONBLOCK) < 0 )
	{
		std::cerr << "***Error: process::start: pipe: write fd open failed: " << strerror(errno) << std::endl;
		close(m_impl->m_cwpr_fd[0]);
		close(m_impl->m_cwpr_fd[1]);
		return false;
	}

	m_impl->m_pid = vfork();
	if( m_impl->m_pid < 0 )
	{
		std::cerr << "***Error: process::start: fork failed: " << strerror(errno) << std::endl;
		return false;
	}

	else if( m_impl->m_pid == 0 ) //child
	{
		fcntl(m_impl->m_cwpr_fd[1], F_SETFL, fcntl(m_impl->m_cwpr_fd[1], F_GETFL) & ~O_NONBLOCK);
		dup2(m_impl->m_pwcr_fd[0], STDIN_FILENO);
		dup2(m_impl->m_cwpr_fd[1], STDOUT_FILENO);

		int res = chdir(m_impl->m_work_path.c_str());
		GTS_UNUSED(res);

		for(auto &env : m_impl->m_env)
			setenv(env.first.c_str(), env.second.c_str(), 1);

		const char **_args = new const char*[args.size() + 2] {m_impl->m_file_name.c_str(), nullptr};
		for(std::size_t i=0; i<args.size(); i++)
		{
			if( not args[i].empty() )
				_args[i+1] = args[i].c_str();
		}

		res = execvp(m_impl->m_file_name.c_str(), const_cast<char**>(_args));
		delete[] _args;

		perror("***Error: execvp");
		_exit(res);
	}

	m_impl->m_read_fd = std::make_shared<process_impl::descriptor>(m_impl->m_io, m_impl->m_cwpr_fd[0]);
	m_impl->m_write_fd = std::make_shared<process_impl::descriptor>(m_impl->m_io, m_impl->m_pwcr_fd[1]);

//	g_mutex.lock();
	if( m_impl->m_pid > 0 )
	{
		g_process_map.emplace(m_impl->m_pid, this);
		return true;
	}
//	g_mutex.unlock();
	return false;
}

void process::terminate()
{
	if( m_impl->m_pid > 0 )
		::kill(m_impl->m_pid, SIGTERM);
}

void process::kill()
{
	if( m_impl->m_pid > 0 )
		::kill(m_impl->m_pid, SIGKILL);
}

static bool wait(int fd, int event, const std::chrono::milliseconds &ms, asio::error_code &error)
{
	struct pollfd fds;
	fds.fd = fd;
	fds.events = event;

	int tt = ms.count() > 0? ms.count() : -1;
	error == std::make_error_code(static_cast<std::errc>(0));

	for(;;)
	{
		int res = poll(&fds, 1, tt);
		if( res < 0 )
		{
			error == std::make_error_code(static_cast<std::errc>(errno));
			return false;
		}
		else if( res == 0 )
			return false;
		else if( res == 1 and fds.revents == event )
			break;
	}
	return true;
}

bool process::wait_writeable(const duration &ms, asio::error_code &error)
{
	return wait(m_impl->m_write_fd->native_handle(), POLLOUT, ms, error);
}

bool process::wait_readable(const duration &ms, asio::error_code &error)
{
	return wait(m_impl->m_read_fd->native_handle(), POLLIN, ms, error);
}

bool process::wait_writeable(const duration &ms)
{
	asio::error_code error ;
	bool res = wait(m_impl->m_write_fd->native_handle(), POLLOUT, ms, error);
	if( error )
		this->error(error, "wait_writeable");
	return res;
}

bool process::wait_readable(const duration &ms)
{
	asio::error_code error ;
	bool res = wait(m_impl->m_read_fd->native_handle(), POLLIN, ms, error);
	if( error )
		this->error(error, "wait_readable");
	return res;
}

std::size_t process::read_some(void *buf, std::size_t size, asio::error_code &error)
{
	if( m_impl->m_pid > 0 )
	{
		m_impl->m_read_fd->non_blocking(false);
		return m_impl->m_read_fd->write_some(asio::buffer(buf, size), error);
	}
	error = std::make_error_code(std::errc::bad_file_descriptor);
	return 0;
}

void process::async_write_some(const char *buf, std::size_t size, std::function<void(asio::error_code,std::size_t)> call_back)
{
	if( m_impl->m_pid > 0 )
	{
		m_impl->m_write_fd->non_blocking(true);
		m_impl->m_write_fd->async_write_some(asio::buffer(buf, size), std::move(call_back));
	}
	else
	{
		m_impl->m_io.post([call_back]{
			call_back(std::make_error_code(std::errc::operation_canceled), 0);
		});
	}
}

void process::async_read_some(char *buf, std::size_t size, std::function<void(asio::error_code,std::size_t)> call_back)
{
	if( m_impl->m_read_fd == nullptr )
	{
		m_impl->m_io.post([call_back]{
			call_back(std::make_error_code(std::errc::operation_canceled), 0);
		});
		return ;
	}

	m_impl->m_read_fd->non_blocking(true);
	int res = ::read(m_impl->m_read_fd->native_handle(), buf, size);

	if( res > 0 )
	{
		m_impl->m_io.post([call_back, res]{
			call_back({}, static_cast<std::size_t>(res));
		});
		return ;
	}
	else if( res == 0 )
	{
		if( m_impl->m_pid < 0 )
		{
			m_impl->m_io.post([call_back]{
				call_back(std::make_error_code(std::errc::operation_canceled), 0);
			});
			return m_impl->reset_reader();
		}
	}
	else if( errno != EAGAIN and errno != EWOULDBLOCK )
	{
		m_impl->m_io.post([call_back]{
			call_back(std::make_error_code(static_cast<std::errc>(errno)), 0);
		});

		if( m_impl->m_pid < 0 )
			m_impl->reset_reader();
		return ;
	}
	if( not is_running() )
	{
		m_impl->reset_reader();
		m_impl->m_io.post([call_back, res]{
			call_back(std::make_error_code(std::errc::operation_canceled), 0);
		});
		return ;
	}

	// If the peer is closed, the callback function should theoretically be fired immediately
	// ith size equal to 0, but in practice the callback function is never called.
	m_impl->m_read_fd->async_read_some
			(asio::buffer(buf, size),
			 [this, call_back](const asio::error_code &error, std::size_t size)
	{
		call_back(error, size);
		if( size == 0 )
			m_impl->reset_reader();
	});
}

void process::cancel()
{
	if( m_impl->m_pid <= 0 )
		return ;

	m_impl->m_write_fd->cancel();
	m_impl->m_read_fd->cancel();
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
	return m_impl->m_pid > 0;
}

bool process::join(int *ret_val)
{
	if( m_impl->m_pid <= 0 )
	{
		if( ret_val )
			*ret_val = 0;
		return false;
	}
	int status = 0;
	pid_t pid = waitpid(m_impl->m_pid, &status, 0);
	m_impl->m_pid = -1;

	if( pid > 0 )
		m_impl->reset_writer(pid);
	if( WIFEXITED(status) )
	{
		if( ret_val )
			*ret_val = WEXITSTATUS(status);
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------------------------------------------------*/

static __sighandler_t pre_signal_hander = signal(SIGCHLD, &process_impl::signal_hander);

void process_impl::signal_hander(int signo)
{
	if( signo == SIGCHLD )
	{
		int status = 0;
		pid_t pid = waitpid(-1, &status, WNOHANG);

//		g_mutex.lock();
		auto it = g_process_map.find(pid);
		if( it != g_process_map.end() )
		{
			it->second->m_impl->m_pid = -1;
			it->second->m_impl->reset_writer(pid);
		}
//		g_mutex.unlock();
	}

	else if( pre_signal_hander )
		pre_signal_hander(signo);
}

} //namespace gts

#endif //__unix__
