#include "gts_global.h"

namespace gts
{

asio::io_context &io_context()
{
	static asio::io_context io;
	return io;
}

} //namespace gts
