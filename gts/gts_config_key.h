#ifndef GTS_CONFIG_KEY_H
#define GTS_CONFIG_KEY_H

#define SINI_GROUP_GTS  "gts"

#define SINI_GTS_SITES_CONFIG    "sites_config_file"

#define SINI_GTS_TCP_BUF_SIZE    "tcp_buffer_size"

#define SINI_GTS_SSL_CRT_FILE    "ssl_crt_file"

#define SINI_GTS_SSL_KEY_FILE    "ssl_key_file"

#define SINI_GTS_SSL_KEY         "ssl_key"

#define SINI_GTS_PLUGINS_CONFIG  "plugins_config"


#define SINI_GROUP_GTSLOG  "gtslog"

#define SINI_GTS_LOG_DIR       "directory"

#define SINI_GTS_LOG_DIR_TMCY  "time_category"

#define SINI_GTS_LOG_LEVEL     "level_mask"

#define SINI_GTS_LOG_ASYNC     "async"

#define SINI_GTS_LOG_MAXOF     "max_size_one_file"

#define SINI_GTS_LOG_MAXOD     "max_size_one_day"

#define SINI_GTS_LOG_MAX       "max_size"


#ifdef _WINDOWS
# define _GTS_SSL_CRT_DEFAULT_FILE  ""
# define _GTS_SSL_KEY_DEFAULT_FILE  ""
#elif defined(__unix__)
# define _GTS_SSL_CRT_DEFAULT_FILE  "~/.ssl/ssl.crt"
# define _GTS_SSL_KEY_DEFAULT_FILE  "~/.ssl/ssl.key"
#else //other os
# define _GTS_SSL_CRT_DEFAULT_FILE  ""
# define _GTS_SSL_KEY_DEFAULT_FILE  ""
#endif //os

#define _GTS_DEFAULT_SITES_CONFIG    "sites.json"
#define _GTS_DEFAULT_PLUGINS_CONFIG  "../plugins.json"


#endif //GTS_CONFIG_KEY_H