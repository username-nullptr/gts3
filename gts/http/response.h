
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

#ifndef GTS_HTTP_RESPONSE_H
#define GTS_HTTP_RESPONSE_H

#include <gts/http/types.h>
#include <gts/tcp_socket.h>
#include <functional>

namespace gts { namespace http
{

class request;
class response_impl;
class file_transfer;

class GTS_HTTP_API response
{
	GTS_DISABLE_COPY_MOVE(response)

	template <typename T>
	using is_string = http::value::is_string<T>;

	template <typename CT>
	using not_value_t = enable_if_t<not is_string<CT>::value, int>;

	template <typename CT>
	using not_cookie_t = enable_if_t<not gts_is_dsame(CT, http::cookie) and not is_string<CT>::value, int>;

public:
	explicit response(http::request &request, http::status status = hs_ok);
	response(http::request &request, const http::headers &headers, http::status status = hs_ok);
	~response();

public:
	response &set_status(int status);
	response &set_headers(const http::headers &headers);
	response &set_cookies(const http::cookies &cookies);

public:
	response &set_header(std::string key, http::value value);

	template <typename...Args>
	response &set_header(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

public:
	response &set_cookie(std::string key, http::cookie cookie);

	template <typename...Args>
	response &set_cookie(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

public:
	std::string version() const;
	http::status status() const;

public:
	const http::headers &headers() const;
	http::headers &headers();
	const http::cookies &cookies() const;
	http::cookies &cookies();

public:
	bool headers_contains(const std::string &key) const;
	value &header(const std::string &key) noexcept(false);
	const value &header(const std::string &key) const noexcept(false);
	value header_or(const std::string &key, value deft_value = {}) const noexcept;

	template <typename T>
	T header(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	T header_or(const std::string &key, T deft_value) const noexcept;

public:
	bool cookies_contains(const std::string &key) const;
	http::cookie &cookie(const std::string &key) noexcept(false);
	const http::cookie &cookie(const std::string &key) const noexcept(false);
	http::cookie cookie_or(const std::string &key, http::cookie deft_value = {}) const noexcept;

	template <typename T>
	T cookie(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_cookie_t<T>>
	T cookie_or(const std::string &key, T deft_value) const noexcept;

public:
	response &write_default();
	response &write_default(int status);

public:
	response &write();
	response &write(std::size_t size, const void *body);
	response &write(const std::string &body);
	response &write(const char *body);

	template <typename...Args>
	response &write(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T, typename U = not_cookie_t<T>>
	response &write(T &&value);

public:
	response &set_chunk_attribute(http::value attribute);
	response &set_chunk_attributes(value_list attributes);

	template <typename...Args>
	response &set_chunk_attribute(fmt::format_string<Args...> fmt, Args&&...args);
	response &chunk_end(const http::headers &headers = {});

public:
	response &write_file(const std::string &file_name);
	response &write_file(const std::string &file_name, const std::string &range_http_value);
	response &write_file(const std::string &file_name, std::size_t begin, std::size_t end);

public:
	struct range
	{
		std::size_t begin;
		std::size_t end;
	};
	using range_vector = std::vector<range>;
	response &write_file(const std::string &file_name, const response::range &range);
	response &write_file(const std::string &file_name, const range_vector &range) noexcept(false);

public:
	response &redirect(const std::string &url, redirect_type type = redirect_type::moved_permanently);

public:
	response &unset_header(const std::string &key);
	response &unset_cookie(const std::string &key);
	void close(bool force = false);

public:
	static void set_resource_root(const std::string &path);
	static std::string resource_root();

public:
	static void set_default_write(std::function<void(response&)>);
	bool is_writed() const;

public:
	tcp_socket_ptr take() const;
	bool is_valid() const;

private:
	friend class file_transfer;
	response_impl *m_impl;
};

}} //namespace gts::http

#include <gts/http/detail/response.h>

#endif //GTS_HTTP_RESPONSE_H
