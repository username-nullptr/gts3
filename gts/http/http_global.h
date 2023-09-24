#ifndef GTS_HTTP_GLOBAL_H
#define GTS_HTTP_GLOBAL_H

#include <gts/gts_global.h>

#ifdef gtshttp_EXPORTS
# define GTS_HTTP_API  GTS_DECL_EXPORT
#else //gtshttp_EXPORTS
# define GTS_HTTP_API  GTS_DECL_IMPORT
#endif //gtshttp_EXPORTS


#endif //GTS_HTTP_GLOBAL_H
