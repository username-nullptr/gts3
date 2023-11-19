
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

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
# define GTS_SSL_CRT_DEFAULT_FILE  ""
# define GTS_SSL_KEY_DEFAULT_FILE  ""
#elif defined(__unix__)
# define GTS_SSL_CRT_DEFAULT_FILE  "~/.ssl/ssl.crt"
# define GTS_SSL_KEY_DEFAULT_FILE  "~/.ssl/ssl.key"
#else //other os
# define GTS_SSL_CRT_DEFAULT_FILE  ""
# define GTS_SSL_KEY_DEFAULT_FILE  ""
#endif //os

#define GTS_DEFAULT_SITES_CONFIG    "sites.json"
#define GTS_DEFAULT_PLUGINS_CONFIG  "../plugins.json"


#endif //GTS_CONFIG_KEY_H
