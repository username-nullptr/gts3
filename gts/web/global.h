#ifndef GTS_WEB_GLOBAL_H
#define GTS_WEB_GLOBAL_H

#include <gts/global.h>

#ifdef gtsweb_EXPORTS
# define GTS_WEB_API  GTS_DECL_EXPORT
#else //gtsweb_EXPORTS
# define GTS_WEB_API  GTS_DECL_IMPORT
#endif //gtsweb_EXPORTS

namespace gts { namespace web
{

GTS_WEB_API std::string resource_root();

}} //namespace gts::web


#endif //GTS_WEB_GLOBAL_H
