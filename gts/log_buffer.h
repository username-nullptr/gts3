
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

#ifndef GTS_LOG_BUFFER_H
#define GTS_LOG_BUFFER_H

#include <gts/global.h>
#include <cppformat>
#include <chrono>
#include <cstdio>

GTS_NAMESPACE_BEGIN

class logger;

class GTSCORE_API log_buffer
{
public:
	enum type
	{
		debug,   //stdout
		info,    //stdout
		warning, //stderr
		error,   //stderr
		fetal    //stderr-sync
	};

	using otime = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
	struct context
	{
		otime time;
		std::string category;
		const char *file;
		const char *func;
		int line;

		context() = default;
		context(const context &other) = default;
		context &operator=(const context &other) = default;
		context(context &&other) noexcept;
		context &operator=(context &&other) noexcept;
	};

public:
	explicit log_buffer(type t);
	log_buffer(const log_buffer &other);
	log_buffer(log_buffer &&other) noexcept;
	~log_buffer();

public:
	log_buffer &operator=(const log_buffer &other);
	log_buffer &operator=(log_buffer &&other) noexcept;

public:
	template <typename T>
	log_buffer &write(T &&msg);

	template <typename T>
	log_buffer &operator<<(T &&msg);

private:
	struct data
	{
		log_buffer::type type;
		log_buffer::context context;
		std::string buffer;
	}
	*m_data;

private:
	friend class logger;
};

GTS_NAMESPACE_END
#include <gts/detail/log_buffer.h>

#endif //GTS_LOG_BUFFER_H
