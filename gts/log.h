
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

#ifndef GTS_LOG_H
#define GTS_LOG_H

#include <gts/log_buffer.h>
#include <gts/formatter.h>

GTS_NAMESPACE_BEGIN

#define gts_log_debug    gts::logger(__FILE__, __func__, __LINE__).debug
#define gts_log_info     gts::logger(__FILE__, __func__, __LINE__).info
#define gts_log_warning  gts::logger(__FILE__, __func__, __LINE__).warning
#define gts_log_error    gts::logger(__FILE__, __func__, __LINE__).error
#define gts_log_fatal    gts::logger(__FILE__, __func__, __LINE__).fatal

#define gts_custom_log_debug    gts::logger(__FILE__, __func__, __LINE__).cdebug
#define gts_custom_log_info     gts::logger(__FILE__, __func__, __LINE__).cinfo
#define gts_custom_log_warning  gts::logger(__FILE__, __func__, __LINE__).cwarning
#define gts_custom_log_error    gts::logger(__FILE__, __func__, __LINE__).cerror
#define gts_custom_log_fatal    gts::logger(__FILE__, __func__, __LINE__).cfatal

class logger_impl;

class GTSCORE_API logger
{
	GTS_DISABLE_COPY_MOVE(logger)
	using duration = std::chrono::milliseconds;

public:
	struct context
	{
		std::string dir      = "";
		std::string category = "default";

		std::size_t max_size_one_file = 10240;
		std::size_t max_size_one_day  = 10485760;
		std::size_t max_size          = 1073741824;

		int  mask          = 4;
		int  time_category = true;
		bool async         = false;
		bool no_stdout     = false;
	};

public:
	logger(const char *file, const char *func, int line);
	~logger();

public:
	static void set_context(const context &con);
	static void set_header_breaks_aline(bool enable = true); //default false

public:
	static context get_context();
	static bool get_header_breaks_aline();

public:
	template <typename...Args>
	log_buffer debug(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer debug(T &&msg);
	log_buffer debug();

	template <typename...Args>
	log_buffer info(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer info(T &&msg);
	log_buffer info();

	template <typename...Args>
	log_buffer warning(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer warning(T &&msg);
	log_buffer warning();

	template <typename...Args>
	log_buffer error(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer error(T &&msg);
	log_buffer error();

	template <typename...Args>
	void fatal(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	void fatal(T &&msg);

public:
	template <typename...Args>
	log_buffer cdebug(std::string category, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer cdebug(std::string category, T &&msg);
	log_buffer cdebug(std::string category);

	template <typename...Args>
	log_buffer cinfo(std::string category, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer cinfo(std::string category, T &&msg);
	log_buffer cinfo(std::string category);

	template <typename...Args>
	log_buffer cwarning(std::string category, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer cwarning(std::string category, T &&msg);
	log_buffer cwarning(std::string category);

	template <typename...Args>
	log_buffer cerror(std::string category, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	log_buffer cerror(std::string category, T &&msg);
	log_buffer cerror(std::string category);

	template <typename...Args>
	void cfatal(std::string category, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	void cfatal(std::string category, T &&msg);

public:
	static void wait(const duration &ms);
	static void reload();

private:
	void _fatal(const std::string &msg, std::string category = "");
	logger_impl *m_impl;
};

GTS_NAMESPACE_END
#include <gts/detail/log.h>

#endif //GTS_LOG_H
