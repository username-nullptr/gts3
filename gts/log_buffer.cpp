
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

#include "log_buffer.h"
#include <thread>

GTS_NAMESPACE_BEGIN

log_buffer::context::context(context &&other) noexcept
{
	operator=(std::move(other));
}

log_buffer::context &log_buffer::context::operator=(context &&other) noexcept
{
	time     = other.time;
	category = std::move(other.category);
	file     = other.file;
	func     = other.func;
	line     = other.line;
	return *this;
}

log_buffer::log_buffer(type t) :
	m_data(new data)
{
	m_data->type = t;
}

log_buffer::log_buffer(const log_buffer &other) :
	m_data(new data)
{
	operator=(other);
}

log_buffer::log_buffer(log_buffer &&other) noexcept
{
	m_data = other.m_data;
	other.m_data = nullptr;
}

log_buffer &log_buffer::operator=(const log_buffer &other)
{
	*m_data = *other.m_data;
	return *this;
}

log_buffer &log_buffer::operator=(log_buffer &&other) noexcept
{
	if( m_data )
		delete m_data;
	m_data = other.m_data;
	other.m_data = nullptr;
	return *this;
}

extern void message_handler(log_buffer::type type, log_buffer::context &&runtime_context, std::string &&msg);

log_buffer::~log_buffer()
{
	if( m_data == nullptr )
		return ;

#ifndef __NO_DEBUG__
	else if( not m_data->buffer.empty() )
		message_handler(m_data->type, std::move(m_data->context), std::move(m_data->buffer));
#endif //__NO_DEBUG__
	delete m_data;
}

GTS_NAMESPACE_END
