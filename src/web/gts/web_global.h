#ifndef GTS_WEB_GLOBAL_H
#define GTS_WEB_GLOBAL_H

#include <gts/gts_global.h>

#ifdef gtsweb_EXPORTS
# define GTSWEB_API  GTS_DECL_EXPORT
#else //gtsweb_EXPORTS
# define GTSWEB_API  GTS_DECL_IMPORT
#endif //gtsweb_EXPORTS

namespace gts { namespace web
{

GTSWEB_API std::string resource_root();

class http_parser;
class service_io;
class session;

}} //namespace gts::web


#endif //GTS_WEB_GLOBAL_H
