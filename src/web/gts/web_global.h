#ifndef GTS_WEB_GLOBAL_H
#define GTS_WEB_GLOBAL_H

#include <gts/gts_global.h>
#include <functional>

#ifdef gtsweb_EXPORTS
# define GTSWEB_API  GTS_DECL_EXPORT
#else //gtsweb_EXPORTS
# define GTSWEB_API  GTS_DECL_IMPORT
#endif //gtsweb_EXPORTS

namespace gts { namespace web
{

#ifdef USING_ASIO
GTSWEB_API asio::thread_pool &thread_pool();
#endif //using asio

GTSWEB_API std::size_t thread_pool_count();

GTSWEB_API void thread_pool_post(std::function<void()> func);

}} //namespace gts::web


#endif //GTS_WEB_GLOBAL_H
