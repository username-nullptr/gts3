
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

#ifndef GTS_DETAIL_LOG_H
#define GTS_DETAIL_LOG_H

namespace gts
{

template <typename...Args> inline
log_buffer logger::debug(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return std::move(debug() << fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline log_buffer logger::debug(std::string msg) {
	return std::move(debug() << std::move(msg));
}

template <typename...Args> inline
log_buffer logger::info(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return std::move(info() << fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline log_buffer logger::info(std::string msg) {
	return std::move(info() << std::move(msg));
}

template <typename...Args> inline
log_buffer logger::warning(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return std::move(warning() << fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline log_buffer logger::warning(std::string msg) {
	return std::move(warning() << std::move(msg));
}

template <typename...Args> inline
log_buffer logger::error(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return std::move(error() << fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline log_buffer logger::error(std::string msg) {
	return std::move(error() << std::move(msg));
}

template <typename...Args> inline
void logger::fatal(fmt::format_string<Args...> fmt_value, Args&&...args) {
	_fatal(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void logger::fatal(const std::string &msg) {
	_fatal(msg);
}

template <typename...Args> inline
log_buffer logger::cdebug(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return std::move(cdebug(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...));
}

inline log_buffer logger::cdebug(std::string category, std::string msg) {
	return std::move(cdebug(std::move(category)) << std::move(msg));
}

template <typename...Args> inline
log_buffer logger::cinfo(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return std::move(cinfo(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...));
}

inline log_buffer logger::cinfo(std::string category, std::string msg) {
	return std::move(cinfo(std::move(category)) << std::move(msg));
}

template <typename...Args> inline
log_buffer logger::cwarning(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return std::move(cwarning(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...));
}

inline log_buffer logger::cwarning(std::string category, std::string msg) {
	return std::move(cwarning(std::move(category)) << std::move(msg));
}

template <typename...Args> inline
log_buffer logger::cerror(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return std::move(cerror(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...));
}

inline log_buffer logger::cerror(std::string category, std::string msg) {
	return std::move(cerror(std::move(category)) << std::move(msg));
}

template <typename...Args> inline
void logger::cfatal(std::string category, fmt::format_string<Args...> fmt_value, Args&&...args) {
	_fatal(fmt::format(fmt_value, std::forward<Args>(args)...), std::move(category));
}

inline void logger::cfatal(std::string category, const std::string &msg) {
	_fatal(msg, std::move(category));
}

} //namespace gts


#endif //GTS_DETAIL_LOG_H
