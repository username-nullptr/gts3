
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

#include "detail/socket_frame_impl.h"
#include "socket_frame.h"

GTS_WEB_NAMESPACE_BEGIN

socket_frame::socket_frame() :
	m_impl(new socket_frame_impl())
{

}

socket_frame::~socket_frame()
{
	if( m_impl )
		delete m_impl;
}

socket_frame::socket_frame(socket_frame &&other) noexcept :
	m_impl(other.m_impl)
{
	other.m_impl = nullptr;
}

socket_frame &socket_frame::operator=(socket_frame &&other) noexcept
{
	if( m_impl )
		delete m_impl;

	m_impl = other.m_impl;
	other.m_impl = nullptr;
	return *this;
}

socket_frame &socket_frame::swap(socket_frame &other)
{
	std::swap(m_impl, other.m_impl);
	return *this;
}

bool socket_frame::is_final_frame() const
{
	return m_impl and m_impl->m_final_frame;
}

bool socket_frame::is_data_frame() const
{
	return m_impl and m_impl->m_data_frame;
}

bool socket_frame::is_control_frame() const
{
	return m_impl and (m_impl->m_op_code & 0x08) == 0x08;
}

bool socket_frame::is_continuation_frame() const
{
	return m_impl and m_impl->m_continuation_frame;
}

bool socket_frame::rsv1() const
{
	return m_impl and m_impl->m_rsv1;
}

bool socket_frame::rsv2() const
{
	return m_impl and m_impl->m_rsv2;
}

bool socket_frame::rsv3() const
{
	return m_impl and m_impl->m_rsv3;
}

bool socket_frame::has_mask() const
{
	return m_impl and m_impl->m_mask > 0;
}

uint32_t socket_frame::mask() const
{
	return m_impl ? m_impl->m_mask : 0;
}

sp_close_code socket_frame::close_code() const
{
	return m_impl ? m_impl->m_close_code : static_cast<sp_close_code>(0);
}

sp_op_code socket_frame::op_code() const
{
	return m_impl ? m_impl->m_op_code : static_cast<sp_op_code>(0);
}

std::string socket_frame::payload() const
{
	return m_impl ? m_impl->m_payload : "";
}

uint64_t socket_frame::length() const
{
	return m_impl ? m_impl->m_length : 0;
}

bool socket_frame::is_valid() const
{
	return m_impl and m_impl->m_valid;
}

GTS_WEB_NAMESPACE_END
