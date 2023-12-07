
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

#include "socket_frame_parser.h"

GTS_WEB_NAMESPACE_BEGIN

enum class status
{
	read_header,
	read_payload_length,
	read_mask,
	read_payload,
	dispatch_result,
	wait_for_more_data
};

class GTS_DECL_HIDDEN socket_frame_parser_impl
{
	GTS_DISABLE_COPY_MOVE(socket_frame_parser_impl)

public:
	status read_header()
	{

	}

	status read_payload_length()
	{

	}

	status read_mask()
	{

	}

	status read_payload()
	{

	}

public:
	std::string m_abuf;
};

/*-------------------------------------------------------------------------------------------------------------------*/

bool socket_frame_parser::is_final_frame() const
{
	return false;
}

bool socket_frame_parser::is_control_frame() const
{
	return false;
}

bool socket_frame_parser::is_data_frame() const
{
	return false;
}

bool socket_frame_parser::is_continuation_frame() const
{
	return false;
}

bool socket_frame_parser::rsv1() const
{
	return false;
}

bool socket_frame_parser::rsv2() const
{
	return false;
}

bool socket_frame_parser::rsv3() const
{
	return false;
}

bool socket_frame_parser::has_mask() const
{
	return false;
}

uint32_t socket_frame_parser::mask() const
{
	return 0;
}

socket_protocol::close_code socket_frame_parser::close_code() const
{
	return socket_protocol::cc_missing_status_code;
}

socket_protocol::op_code socket_frame_parser::op_code() const
{
	return socket_protocol::oc_reserved_d;
}

std::string socket_frame_parser::payload() const
{
	return "";
}

bool socket_frame_parser::is_valid() const
{
	return false;
}

bool socket_frame_parser::is_done() const
{
	return false;
}

bool socket_frame_parser::load(const std::string &data)
{
	return false;
}

void socket_frame_parser::clear()
{

}

void socket_frame_parser::set_max_allowed_frame_size(std::size_t size)
{

}

std::size_t socket_frame_parser::max_allowed_frame_size() const
{
	return 0;
}

std::size_t socket_frame_parser::max_frame_size()
{
	return 0;
}

GTS_WEB_NAMESPACE_END

