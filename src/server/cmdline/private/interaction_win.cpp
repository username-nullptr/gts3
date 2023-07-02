#ifdef _WINDOWS

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

namespace gts { namespace cmdline
{

interaction_private::interaction_private(bool is_server)
{

}

interaction_private::~interaction_private()
{

}

/*---------------------------------------------------------------------------------------*/

bool interaction::open()
{
}

void interaction::close()
{
}

bool interaction::is_open() const
{
}

int interaction::read(char *buf, int len, int timeout)
{
}

int interaction::write(const char *buf, int len, int timeout)
{
}

void interaction::async_read(char *buf, int len, std::function<void(int)> call_back)
{
}

void interaction::async_write(char *buf, int len, std::function<void(int)> call_back)
{
}

void interaction::cancel()
{
}

}} // namespace gts::cmdline

#endif //_WINDOWS
