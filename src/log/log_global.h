#ifndef LOG_GLOBAL_H
#define LOG_GLOBAL_H

#include "gts_global.h"

#ifdef gtslog_EXPORTS
# define GTSLOG_API  GTS_DECL_EXPORT
#else
# define GTSLOG_API  GTS_DECL_IMPORT
#endif //gtslog_EXPORTS


#endif //LOG_GLOBAL_H
