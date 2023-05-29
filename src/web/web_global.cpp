#include "web_global.h"

namespace gts { namespace web
{

asio::io_context &io_context()
{
	static asio::io_context io;
	return io;
}

}} //namespace gts::web
