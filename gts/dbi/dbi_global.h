#ifndef GTS_DBI_GLOBAL_H
#define GTS_DBI_GLOBAL_H

#include <gts/gts_global.h>

#ifdef gtsdbi_EXPORTS
# define GTS_DBI_API  GTS_DECL_EXPORT
#else //gtsdbi_EXPORTS
# define GTS_DBI_API  GTS_DECL_IMPORT
#endif //gtsdbi_EXPORTS


#endif //GTS_DBI_GLOBAL_H
