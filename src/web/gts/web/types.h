#ifndef GTS_WEB_TYPES_H
#define GTS_WEB_TYPES_H

#include <gts/http/container.h>

namespace gts { namespace web
{

using environment = http::pair<http::value>;

using environments = http::unordered_map<http::value>;

}} //namespace gts::web


#endif //GTS_WEB_TYPES_H
