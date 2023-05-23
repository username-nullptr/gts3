#ifndef GTS_CONFIG_KEY_H
#define GTS_CONFIG_KEY_H

#define SINI_GROUP_GTS  "gts"

#define SINI_GTS_ADDRESS       "address"

#define SINI_GTS_PORT          "port"

#define SINI_GTS_TCP_BUF_SIZE  "tcp_buffer_size"

#define SINI_GTS_STRATEGY      "service_strategy"

#define SINI_GTS_SSCFG         "subserver_config"


#define SINI_GROUP_GTSLOG  "gtslog"

#define SINI_GTS_LOG_DIR       "directory"

#define SINI_GTS_LOG_LEVEL     "level_mask"

#define SINI_GTS_LOG_CATEGORY  "category"

#define SINI_GTS_LOG_ASYNC     "async"

#define SINI_GTS_LOG_MAXOF     "max_size_one_file"

#define SINI_GTS_LOG_MAXOD     "max_size_one_day"

#define SINI_GTS_LOG_MAX       "max_size"

#ifdef _MSC_VER
# define _GTS_DEFULT_STRATEGY       "gtsweb.dll"
#elif defined(__GNUC__)
# define _GTS_DEFULT_STRATEGY       "libgtsweb.so"
#else //
# define _GTS_DEFULT_STRATEGY  "???"
#endif


#endif //GTS_CONFIG_KEY_H
