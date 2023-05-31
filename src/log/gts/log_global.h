#ifndef GTS_LOG_GLOBAL_H
#define GTS_LOG_GLOBAL_H

#include <gts/gts_global.h>

#ifdef gtslog_EXPORTS
# define GTSLOG_API  GTS_DECL_EXPORT
#else
# define GTSLOG_API  GTS_DECL_IMPORT
#endif //gtslog_EXPORTS


#endif //GTS_LOG_GLOBAL_H
