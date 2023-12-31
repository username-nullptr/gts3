#include "subserver_manager.h"
#include "gts/log.h"

#include <cassert>
#include <atomic>

namespace gts { namespace subserver
{

static class GTS_DECL_HIDDEN manager_impl
{
	GTS_DISABLE_COPY_MOVE(manager_impl)

public:
	manager_impl();
}
*g_impl = nullptr;

manager_impl::manager_impl()
{

}

/*--------------------------------------------------------------------------------------------------*/

static manager *g_instance = nullptr;
static std::atomic_int g_counter {0};

manager::manager()
{
	if( g_counter++ == 0 )
	{
		g_impl = new manager_impl();
		g_instance = this;
	}
}

manager::~manager()
{
	if( --g_counter == 0 )
	{
		gts_log_debug("class 'subserver::manager' exit.");
		delete g_impl;
		g_instance = nullptr;
		g_impl = nullptr;
	}
}

manager &manager::instance()
{
	assert(g_instance);
	return *g_instance;
}

}} //namespace gts::subserver
