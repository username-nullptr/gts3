#include "private/process_p.h"
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

process_private::process_private(asio::io_context &io) :
	m_io(io)
{

}

/*----------------------------------------------------------------------------------------------------------------------*/

process::process(asio::io_context &io, const std::string &file_name) :
	d_ptr(new process_private(io))
{
	d_ptr->m_file_name = file_name;
}

process::~process()
{
	if( is_running() )
	{
		kill();
		std::cerr << "*** Warning: ~process: kill the process." << std::endl;
	}
	delete d_ptr;
}

void process::set_file(const std::string &file_name)
{
	if( not is_running() )
		d_ptr->m_file_name = file_name;
}

std::string process::file() const
{
	return d_ptr->m_file_name;
}

void process::add_env(const std::string &key, const std::string &value)
{
	if( is_running() )
		return ;

	auto pair = d_ptr->m_env.emplace(key, value);
	if( pair.second == false )
		pair.first->second = value;
}

void process::set_work_path(const std::string &path)
{
	if( not is_running() )
		d_ptr->m_work_path = path;
}

} //namespace gts
