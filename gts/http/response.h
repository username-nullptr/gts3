
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

GTS_HTTP_NAMESPACE_BEGIN

class request;
class response_impl;
class file_transfer;

class GTS_HTTP_API response
{
	GTS_DISABLE_COPY_MOVE(response)

	template <typename T>
	using is_string = gts::value::is_string<T>;

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
	response &set_header(std::string key, gts::value value);

	template <typename...Args>
	response &set_header(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

public:
	response &set_cookie(std::string key, http::cookie cookie);

	template <typename...Args>
	response &set_cookie(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

public:
	GTS_CXX_NODISCARD("") std::string version() const;
	GTS_CXX_NODISCARD("") http::status status() const;

public:
	GTS_CXX_NODISCARD("") const http::headers &headers() const;
	GTS_CXX_NODISCARD("") http::headers &headers();
	GTS_CXX_NODISCARD("") const http::cookies &cookies() const;
	GTS_CXX_NODISCARD("") http::cookies &cookies();

public:
	GTS_CXX_NODISCARD("") bool headers_contains(const std::string &key) const;
	GTS_CXX_NODISCARD("") value &header(const std::string &key) noexcept(false);
	GTS_CXX_NODISCARD("") const value &header(const std::string &key) const noexcept(false);
	GTS_CXX_NODISCARD("") value header_or(const std::string &key, value deft_value = {}) const noexcept;

	template <typename T>
	GTS_CXX_NODISCARD("") T header(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	GTS_CXX_NODISCARD("") T header_or(const std::string &key, T deft_value) const noexcept;

public:
	GTS_CXX_NODISCARD("") bool cookies_contains(const std::string &key) const;
	GTS_CXX_NODISCARD("") http::cookie &cookie(const std::string &key) noexcept(false);
	GTS_CXX_NODISCARD("") const http::cookie &cookie(const std::string &key) const noexcept(false);
	GTS_CXX_NODISCARD("") http::cookie cookie_or(const std::string &key, http::cookie deft_value = {}) const noexcept;

	template <typename T>
	GTS_CXX_NODISCARD("") T cookie(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_cookie_t<T>>
	GTS_CXX_NODISCARD("") T cookie_or(const std::string &key, T deft_value) const noexcept;

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
	response &set_chunk_attribute(gts::value attribute);
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
	GTS_CXX_NODISCARD("") static std::string resource_root();

public:
	static void set_default_write(std::function<void(response&)>);
	GTS_CXX_NODISCARD("") bool is_writed() const;

public:
	GTS_CXX_NODISCARD("Transfer management of socket")
	tcp_socket_ptr take() const;

	GTS_CXX_NODISCARD("Return false, if socket is disconnected or HTTP is invalid")
	bool is_valid() const;

private:
	friend class file_transfer;
	response_impl *m_impl;
};

GTS_HTTP_NAMESPACE_END
#include <gts/http/detail/response.h>

#endif //GTS_HTTP_RESPONSE_H
