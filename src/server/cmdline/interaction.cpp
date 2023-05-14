#include "private/interaction_p.h"
#include <cstring>

namespace gts::cmdline
{

interaction::interaction(bool is_server) :
	d_ptr(new interaction_private(is_server))
{

}

interaction::~interaction()
{
	delete d_ptr;
}

} //namespace gts::cmdline
