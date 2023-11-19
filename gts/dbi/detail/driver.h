
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

#ifndef GTS_DBI_DETAIL_DRIVER_H
#define GTS_DBI_DETAIL_DRIVER_H

GTS_DBI_NAMESPACE_BEGIN

inline driver::~driver() {}

inline bool driver::is_valid() {
	return false;
}

template <typename...Args>
void driver::set_default_connect_string_info(fmt::format_string<Args...> fmt_value, Args&&...args) {
	set_default_connect_string_info(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void driver::set_auto_commit(bool enable) noexcept(false)
{
	error_code error;
	set_auto_commit(error, enable);
	if( error )
		throw exception(error);
}

inline connection_ptr driver::create_connection(error_code &error) noexcept {
	return create_connection(error, default_connect_info());
}

inline connection_ptr driver::create_connection() noexcept(false)
{
	error_code error;
	auto res = create_connection(error, default_connect_info());
	if( error )
		throw exception(error);
	return res;
}

inline connection_ptr driver::create_connection(const dbi::connect_info &info) noexcept(false)
{
	error_code error;
	auto res = create_connection(error, info);
	if( error )
		throw exception(error);
	return res;
}

inline connection_ptr driver::create_connection(const std::string &info) noexcept(false)
{
	error_code error;
	auto res = create_connection(error, info);
	if( error )
		throw exception(error);
	return res;
}

GTS_DBI_NAMESPACE_END


#endif //GTS_DBI_DETAIL_DRIVER_H
