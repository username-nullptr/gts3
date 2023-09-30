#ifndef GTS_WEB_THREAD_POOL_H
#define GTS_WEB_THREAD_POOL_H

#include <gts/web/global.h>

namespace gts { namespace web
{

GTS_WEB_API asio::thread_pool &thread_pool();

GTS_WEB_API std::size_t thread_pool_count();

}} //namespace gts::web


#endif //GTS_WEB_THREAD_POOL_H
