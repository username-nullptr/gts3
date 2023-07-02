#ifdef _WINDOWS

#include "process_p.h"
#include <cassert>
#include <mutex>
#include <map>

namespace gts
{

process_impl::~process_impl()
{

}

/*---------------------------------------------------------------------------------------------------------------*/

bool process::start(const string_list& args)
{

}

void process::terminate()
{
}

void process::kill()
{
}

int process::write(const char *buf, int size, int timeout)
{
	GTS_UNUSED(buf);
	GTS_UNUSED(size);
	GTS_UNUSED(timeout);
	return -1;
}

int process::read(const char *buf, int size, int timeout)
{
	GTS_UNUSED(buf);
	GTS_UNUSED(size);
	GTS_UNUSED(timeout);
	return -1;
}

void process::async_write(const char *buf, int size, std::function<void(asio::error_code,std::size_t)> call_back)
{
}

void process::async_read(char *buf, int size, std::function<void(asio::error_code,std::size_t)> call_back)
{
}

void process::cancel()
{
}

bool process::is_running() const
{
}

void process::join()
{
}

} //namespace gts


#endif //_WINDOWS
