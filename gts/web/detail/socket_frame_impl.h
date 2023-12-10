
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

#ifndef GTS_WEB_DETAIL_SOCKET_FRAME_IMPL_H
#define GTS_WEB_DETAIL_SOCKET_FRAME_IMPL_H

#include "gts/web/socket_frame.h"

GTS_WEB_NAMESPACE_BEGIN

using sp_close_code = socket_protocol::close_code;
using sp_op_code = socket_protocol::op_code;

class GTS_DECL_HIDDEN socket_frame_impl
{
public:
	bool m_final_frame = false;
	bool m_data_frame = false;
	bool m_continuation_frame = false;
	uint64_t m_length = 0;

public:
	bool m_rsv1 = false;
	bool m_rsv2 = false;
	bool m_rsv3 = false;
	uint32_t m_mask = 0;

public:
	sp_close_code m_close_code;
	sp_op_code m_op_code;
	std::string m_payload;

public:
	bool m_valid = false;
};

GTS_WEB_NAMESPACE_END


#endif //GTS_WEB_DETAIL_SOCKET_FRAME_IMPL_H
