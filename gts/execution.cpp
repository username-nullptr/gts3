
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

#include "execution.h"
#include "log.h"

GTS_NAMESPACE_BEGIN

std::atomic_int g_exit_code {0};

std::atomic_bool g_run_flag {true};

execution &execution::instance()
{
	static execution obj;
	return obj;
}

asio::io_context &execution::io_context()
{
	static asio::io_context ioc;
	return ioc;
}

int execution::exec()
{
	static std::atomic_bool flag {false};
	if( flag )
	{
		gts_log_error("gts::execution::exec: not reentrant.");
		return -1;
	}
	flag = true;
	auto &ioc = io_context();

	asio::io_context::work io_work(ioc); GTS_UNUSED(io_work);
	for(;;)
	{
		ioc.run();
		if( not g_run_flag )
			break;
		ioc.restart();
	}
	return g_exit_code;
}

void execution::exit(int code)
{
	g_exit_code = code;
	g_run_flag = false;
	io_context().stop();
}

GTS_NAMESPACE_END
