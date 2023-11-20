
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

#ifndef INTERACTION_H
#define INTERACTION_H

#include "gts/global.h"
#include <functional>
#include <string>

GTS_CMDLINE_NAMESPACE_BEGIN

class interaction_private;

class interaction
{
	GTS_DISABLE_COPY_MOVE(interaction)

public:
	explicit interaction(bool is_server = true);
	~interaction();

public:
	bool open();
	void close();

	GTS_CXX_NODISCARD("")
	bool is_open() const;

public:
	ssize_t read(char *buf, ssize_t len, int timeout = -1);
	ssize_t write(const char *buf, ssize_t len, int timeout = -1);

public:
	void async_read(char *buf, ssize_t len, std::function<void(ssize_t)> call_back);
	void async_write(char *buf, ssize_t len, std::function<void(ssize_t)> call_back);
	void cancel();

private:
	friend class interaction_private;
	interaction_private *d_ptr;
};

GTS_CMDLINE_NAMESPACE_END


#endif //INTERACTION_H
