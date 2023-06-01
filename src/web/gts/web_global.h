#ifndef GTS_WEB_GLOBAL_H
#define GTS_WEB_GLOBAL_H

#include <gts/gts_global.h>
#include <vector>

#ifdef gtsweb_EXPORTS
# define GTSWEB_API  GTS_DECL_EXPORT
#else //gtsweb_EXPORTS
# define GTSWEB_API  GTS_DECL_IMPORT
#endif //gtsweb_EXPORTS

namespace gts { namespace web
{

GTSWEB_API asio::thread_pool &thread_pool();

GTSWEB_API std::size_t thread_pool_count();

}} //namespace gts::web


#endif //GTS_WEB_GLOBAL_H
