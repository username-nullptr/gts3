#ifndef GTS_WEB_THREAD_POOL_H
#define GTS_WEB_THREAD_POOL_H

#include <gts/web_global.h>
#include <functional>

namespace gts { namespace web
{

GTSWEB_API asio::thread_pool &thread_pool();

GTSWEB_API std::size_t thread_pool_count();

}} //namespace gts::web


#endif //GTS_WEB_THREAD_POOL_H
