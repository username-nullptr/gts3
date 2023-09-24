#include "gts_global.h"

namespace gts
{

// The destructor will crash after main returns (the program ends) for unknown reasons.
static auto *io = new asio::io_context();

asio::io_context &io_context()
{
	return *io;
}

} //namespace gts
