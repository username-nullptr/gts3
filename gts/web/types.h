
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

#ifndef GTS_WEB_TYPES_H
#define GTS_WEB_TYPES_H

#include <gts/web/global.h>
#include <gts/http/container.h>

GTS_WEB_NAMESPACE_BEGIN

using environment = http::pair<gts::value>;

using environments = http::unordered_map<gts::value>;

GTS_WEB_NAMESPACE_END

GTS_WEB_SOCKET_PROTOCOL_NAMESPACE_BEGIN

enum version
{
	version_unknown = -1,
	version0  = 0,
	version1  = 1,
	version2  = 2,
	version3  = 3,
	version4  = 4,
	version5  = 5,
	version6  = 6,
	version7  = 7,
	version8  = 8,
	version13 = 13,
	version_latest = version13
};

enum close_code
{
	cc_normal                 = 1000,
	cc_going_away             = 1001,
	cc_protocol_error         = 1002,
	cc_datatype_not_supported = 1003,
	cc_reserved_1004          = 1004,
	cc_missing_Status_code    = 1005,
	cc_abnormal_disconnection = 1006,
	cc_wrong_Datatype         = 1007,
	cc_policy_violated        = 1008,
	cc_too_much_data          = 1009,
	cc_missing_extension      = 1010,
	cc_bad_operation          = 1011,
	cc_tls_handshake_failed   = 1015
};

enum class data_type {
	text, binary
};

class GTS_WEB_API buffer
{
public:
	buffer() = default;
	buffer(std::string data, data_type type = data_type::text);
	buffer(void *data, std::size_t size, data_type type = data_type::text);

public:
	buffer(const buffer&) = default;
	buffer(buffer &&other);

public:
	buffer &operator=(const buffer&) = default;
	buffer &operator=(buffer &&other);

public:
	std::string data;
	data_type type;
};

GTS_WEB_SOCKET_PROTOCOL_NAMESPACE_END
#include <gts/web/detail/types.h>

#endif //GTS_WEB_TYPES_H
