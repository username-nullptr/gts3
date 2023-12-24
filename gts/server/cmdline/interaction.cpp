
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

#include "private/interaction_p.h"
#include "gts/coroutine.h"

GTS_CMDLINE_NAMESPACE_BEGIN

interaction::interaction(bool is_server) :
	d_ptr(new interaction_private(is_server))
{

}

interaction::~interaction()
{
	delete d_ptr;
}

ssize_t interaction::coro_await_read(char *buf, ssize_t len, int timeout)
{
	auto &context = this_coro::get();
	ssize_t res = 0;

	async_read(buf, len, [&](ssize_t size)
	{
		res = size;
		context.invoke();
	});
	context.yield();
	return res;
}

ssize_t interaction::coro_await_write(const char *buf, ssize_t len, int timeout)
{
	auto &context = this_coro::get();
	ssize_t res = 0;

	async_write(buf, len, [&](ssize_t size)
	{
		res = size;
		context.invoke();
	});
	context.yield();
	return res;
}

GTS_CMDLINE_NAMESPACE_END
