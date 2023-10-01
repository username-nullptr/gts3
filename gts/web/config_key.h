
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

#ifndef GTS_WEB_CONFIG_KEY_H
#define GTS_WEB_CONFIG_KEY_H

#define SINI_GROUP_WEB  "web"

#define SINI_WEB_DEFAULT_RESOURCE   "default_resource"

#define SINI_WEB_CGI_ENV            "cgi_env"

#define SINI_WEB_CGI_ACCESS         "cgi_access"

#define SINI_WEB_CGI_PATH           "cgi_path"

#define SINI_WEB_RC_PATH            "resource_path"

#define SINI_WEB_IDLE_TIME_TV       "idle_time_tv"

#define SINI_WEB_MAX_IDLE_TIME      "max_idle_time"

#define SINI_WEB_MAX_SESSION_COUNT  "max_session_count"

#define SINI_WEB_THREAD_POOL_TC     "thread_pool_tc"

#define SINI_WEB_ENABLE_WSS         "enable_websocket_server"

#define SINI_WEB_WSS_NAME           "websocket_server_name"

#define SINI_WEB_WSS_PORT           "websocket_server_port"


#define _GTS_WEB_DEFAULT_CGI_PATH        "../cgi/"
#define _GTS_WEB_DEFAULT_RC_PATH         "../resource/"


#endif //GTS_WEB_CONFIG_KEY_H
