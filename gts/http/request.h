
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

#ifndef GTS_HTTP_REQUEST_H
#define GTS_HTTP_REQUEST_H

#include <gts/http/types.h>
#include <gts/log.h>
#include <asio.hpp>

namespace gts { namespace http
{

class parser;
class parser_impl;

class response;
class response_impl;

class service_context;
class service_context_impl;

class request_impl;

class GTS_HTTP_API request
{
	GTS_DISABLE_COPY_MOVE(request)

	template <typename T>
	using is_string = gts::value::is_string<T>;

	template <typename CT>
	using not_value_t = enable_if_t<not is_string<CT>::value, int>;

public:
	request();
	~request();

public:
	http::method method() const;
	std::string version() const;
	std::string path() const;

public:
	std::string parameters_string() const;
	const http::parameters &parameters() const;
	const http::headers &headers() const;
	const basic_cookies &cookies() const;

public:
	template <class Sesn = http::session>
	std::shared_ptr<Sesn> session() const;

	template <class Sesn = http::session>
	std::shared_ptr<Sesn> session(bool create = true);

public:
	bool parameters_contains(const std::string &key) const;
	const value &parameter(const std::string &key) const noexcept(false);
	value parameter_or(const std::string &key, value deft_value = {}) const noexcept;

	template <typename T>
	T parameter(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	T parameter_or(const std::string &key, T deft_value) const noexcept;

public:
	bool headers_contains(const std::string &key) const;
	const value &header(const std::string &key) const noexcept(false);
	value header_or(const std::string &key, value deft_value = {}) const noexcept;

	template <typename T>
	T header(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	T header_or(const std::string &key, T deft_value) const noexcept;

public:
	bool cookies_contains(const std::string &key) const;
	const value &cookie(const std::string &key) const noexcept(false);
	value cookie_or(const std::string &key, value deft_value) const noexcept;

	template <typename T>
	T cookie(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	T cookie_or(const std::string &key, T deft_value) const noexcept;

public:
	std::string read_body(std::error_code &error, std::size_t size = 0);
	std::size_t read_body(std::error_code &error, void *buf, std::size_t size);
	std::string read_body(std::size_t size = 0);
	std::size_t read_body(void *buf, std::size_t size);

public:
	std::string read_all_body(std::error_code &error);
	std::string read_all_body();

public:
	bool save_file(const std::string &file_name, std::error_code &error, std::size_t begin = 0);
	bool save_file(const std::string &file_name, std::size_t begin = 0);
	bool save_file_part(const std::string &file_name, std::error_code &error, std::size_t total_size, std::size_t begin = 0);
	bool save_file_part(const std::string &file_name, std::size_t total_size, std::size_t begin = 0);

public:
	bool is_websocket_handshake() const;
	bool keep_alive() const;
	bool support_gzip() const;
	bool can_read_body() const;
	bool is_valid() const;

public:
	asio::ip::tcp::endpoint remote_endpoint() const;
	asio::ip::tcp::endpoint local_endpoint() const;

private:
	void set_cookie_session_id(std::string id);

private:
	friend class parser;
	friend class parser_impl;
	friend class response;
	friend class response_impl;
	friend class service_context;
	friend class service_context_impl;
	friend class request_impl;
	request_impl *m_impl;
};

}} //namespace gts::http

#include <gts/http/detail/request.h>

#endif //GTS_HTTP_REQUEST_H
