
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

#ifndef GTS_HTTP_DETAIL_RESPONSE_H
#define GTS_HTTP_DETAIL_RESPONSE_H

GTS_HTTP_NAMESPACE_BEGIN

template <typename...Args>
response &response::set_header(std::string key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_header(std::move(key), fmt::format(value_fmt, std::forward<Args>(args)...));
}

template <typename...Args>
response &response::set_cookie(std::string key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_cookie(std::move(key), http::cookie::from(value_fmt, std::forward<Args>(args)...));
}

template <typename T>
T response::header(const std::string &key) const noexcept(false) {
	return header(key).get<T>();
}

template <typename T, typename U>
T response::header_or(const std::string &key, T deft_value) const noexcept {
	return header_or(key, gts::value(std::move(deft_value))).get<T>();
}

template <typename T>
T response::cookie(const std::string &key) const noexcept(false) {
	return cookie(key).get<T>();
}

template <typename T, typename U>
T response::cookie_or(const std::string &key, T deft_value) const noexcept {
	return cookie_or(key, http::cookie(std::move(deft_value))).get<T>();
}

inline response &response::write_default(int status) {
	return set_status(status).write_default();
}

inline response &response::write() {
	return write(0,nullptr);
}

inline response &response::write(const std::string &body) {
	return write(body.size(), body.c_str());
}

inline response &response::write(const char *body) {
	return write(strlen(body), body);
}

template <typename...Args>
response &response::write(fmt::format_string<Args...> value_fmt, Args&&...args)
{
	auto body = fmt::format(value_fmt, std::forward<Args>(args)...);
	return write(body.size(), body.c_str());
}

template <typename T, typename U>
response &response::write(T &&value) {
	return write("{}", value);
}

template <typename...Args>
response &response::set_chunk_attribute(fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_chunk_attribute(fmt::format(value_fmt, std::forward<Args>(args)...));
}

inline response &response::write_file(const std::string &file_name, const range &_range) {
	return write_file(file_name, range_vector{_range});
}

inline response &response::write_file(const std::string &file_name, std::size_t begin, std::size_t end) {
	return write_file(file_name, range_vector{{begin, end}});
}

GTS_HTTP_NAMESPACE_END


#endif //GTS_HTTP_DETAIL_RESPONSE_H
