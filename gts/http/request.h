
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

GTS_HTTP_NAMESPACE_BEGIN

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
	GTS_CXX_NODISCARD("") http::method method() const;
	GTS_CXX_NODISCARD("") std::string version() const;
	GTS_CXX_NODISCARD("") std::string path() const;

public:
	GTS_CXX_NODISCARD("") std::string parameters_string() const;
	GTS_CXX_NODISCARD("") const http::parameters &parameters() const;
	GTS_CXX_NODISCARD("") const http::headers &headers() const;
	GTS_CXX_NODISCARD("") const basic_cookies &cookies() const;

public:
	template <class Sesn = http::session>
	GTS_CXX_NODISCARD("") std::shared_ptr<Sesn> session() const;

	template <class Sesn = http::session>
	GTS_CXX_NODISCARD("") std::shared_ptr<Sesn> session(bool create = true);

public:
	GTS_CXX_NODISCARD("") bool parameters_contains(const std::string &key) const;
	GTS_CXX_NODISCARD("") const value &parameter(const std::string &key) const noexcept(false);
	GTS_CXX_NODISCARD("") value parameter_or(const std::string &key, value deft_value = {}) const noexcept;

	template <typename T>
	GTS_CXX_NODISCARD("") T parameter(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	GTS_CXX_NODISCARD("") T parameter_or(const std::string &key, T deft_value) const noexcept;

public:
	GTS_CXX_NODISCARD("") bool headers_contains(const std::string &key) const;
	GTS_CXX_NODISCARD("") const value &header(const std::string &key) const noexcept(false);
	GTS_CXX_NODISCARD("") value header_or(const std::string &key, value deft_value = {}) const noexcept;

	template <typename T>
	GTS_CXX_NODISCARD("") T header(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	GTS_CXX_NODISCARD("") T header_or(const std::string &key, T deft_value) const noexcept;

public:
	GTS_CXX_NODISCARD("") bool cookies_contains(const std::string &key) const;
	GTS_CXX_NODISCARD("") const value &cookie(const std::string &key) const noexcept(false);
	GTS_CXX_NODISCARD("") value cookie_or(const std::string &key, value deft_value) const noexcept;

	template <typename T>
	GTS_CXX_NODISCARD("") T cookie(const std::string &key) const noexcept(false);

	template <typename T, typename U = not_value_t<T>>
	GTS_CXX_NODISCARD("") T cookie_or(const std::string &key, T deft_value) const noexcept;

public:
	std::string read_body(std::error_code &error, std::size_t size = 0);
	std::size_t read_body(std::error_code &error, void *buf, std::size_t size);
	std::string read_body(std::size_t size = 0);
	std::size_t read_body(void *buf, std::size_t size);

public:
	std::string read_all_body(std::error_code &error);
	std::string read_all_body();

public:
	std::size_t save_file(const std::string &file_name, std::error_code &error, std::size_t begin = 0);
	std::size_t save_file(const std::string &file_name, std::size_t begin = 0);
	std::size_t save_file_part(const std::string &file_name, std::error_code &error, std::size_t total_size, std::size_t begin = 0);
	std::size_t save_file_part(const std::string &file_name, std::size_t total_size, std::size_t begin = 0);

public:
	GTS_CXX_NODISCARD("") bool is_websocket_handshake() const;
	GTS_CXX_NODISCARD("") bool keep_alive() const;
	GTS_CXX_NODISCARD("") bool support_gzip() const;
	GTS_CXX_NODISCARD("") bool can_read_body() const;
	GTS_CXX_NODISCARD("") bool is_valid() const;

public:
	GTS_CXX_NODISCARD("") asio::ip::tcp::endpoint remote_endpoint() const;
	GTS_CXX_NODISCARD("") asio::ip::tcp::endpoint local_endpoint() const;

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

GTS_HTTP_NAMESPACE_END
#include <gts/http/detail/request.h>

#endif //GTS_HTTP_REQUEST_H
