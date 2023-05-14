#include "global.h"

namespace gts::web
{

asio::io_context &io_context()
{
	static asio::io_context io;
	return io;
}

} //namespace gts::web
