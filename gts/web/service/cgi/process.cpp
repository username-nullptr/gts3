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
namespace gts
{

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
	if( pair.second == false )
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
	asio::error_code error;
	if( wait_readable(timeout, error) )
		return read_some(buf, size);
	else if( error )
		this->error(error, "read_some: wait_readable");
	return 0;
}

std::size_t process::read_some(void *buf, std::size_t size)
{
	asio::error_code error;
	auto res = read_some(buf, size, error);
	if( error )
		this->error(error, "read_some");
	return res;
}

void process::error(const asio::error_code &error, const char *func)
{
	gts_log_error("gts::process::{}: {}", func, error);
}

} //namespace gts
