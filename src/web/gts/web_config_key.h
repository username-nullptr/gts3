#ifndef GTS_WEB_CONFIG_KEY_H
#define GTS_WEB_CONFIG_KEY_H

#define SINI_GROUP_WEB  "web"

#define SINI_WEB_ENABLE_SSL         "enable_ssl"

#define SINI_WEB_SSL_CRT_FILE       "ssl_crt_file"

#define SINI_WEB_SSL_KEY_FILE       "ssl_key_file"

#define SINI_WEB_CGI_ENV            "cgi_env"

#define SINI_WEB_CGI_PATH           "cgi_path"

#define SINI_WEB_PLUGINS_CONFIG     "plugins_config"

#define SINI_WEB_RC_PATH            "resource_path"

#define SINI_WEB_IDLE_TIME_TV       "idle_time_tv"

#define SINI_WEB_MAX_IDLE_TIME      "max_idle_time"

#define SINI_WEB_MAX_SESSION_COUNT  "max_session_count"

#define SINI_WEB_THREAD_POOL_TC     "thread_pool_tc"

#define SINI_WEB_ENABLE_WSS         "enable_websocket_server"

#define SINI_WEB_WSS_NAME           "websocket_server_name"

#define SINI_WEB_WSS_PORT           "websocket_server_port"

#define SINI_WEB_WSS_STRATEGY       "websocket_strategy"


#ifdef _WINDOWS
# define _GTS_WEB_DEFAULT_CGI_PATH        "..\\cgi\\"
# define _GTS_WEB_DEFAULT_RC_PATH         "..\\resource\\"
# define _GTS_WEB_DEFAULT_PLUGINS_CONFIG  "..\\business_plugins/business_plugins.json"
#else //other os
# define _GTS_WEB_DEFAULT_CGI_PATH        "../cgi/"
# define _GTS_WEB_DEFAULT_RC_PATH         "../resource/"
# define _GTS_WEB_DEFAULT_PLUGINS_CONFIG  "../business_plugins/business_plugins.json"
#endif //os

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


#endif //GTS_WEB_CONFIG_KEY_H
