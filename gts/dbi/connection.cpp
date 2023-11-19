
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

#include "connection.h"

GTS_DBI_NAMESPACE_BEGIN

class GTS_DECL_HIDDEN connection_impl
{
	GTS_DISABLE_COPY_MOVE(connection_impl)

public:
	connection_impl() = default;
	std::size_t m_query_string_buffer_size = 1024;
};

/*------------------------------------------------------------------------------------------------------------------------------*/

connection::connection() :
	m_impl(new connection_impl())
{

}

connection::~connection()
{
	delete m_impl;
}

table_data connection::prepare_query(const std::string &statement, error_code &error) noexcept(false)
{
	table_data result;
	result.reserve(64);

	auto set = create_query_work(statement, error);
	if( error )
		return result;

	while( set->next() )
	{
		result.emplace_back();
		auto &vector = result.back();

		vector.reserve(set->column_count());
		auto buf_size = query_string_buffer_size();

		for(std::size_t column=0; column<set->column_count(); column++)
		{
			auto ope = set->get_opt_string(column, buf_size);
			vector.emplace_back(ope.has_value()? cell(set->column_name(column), ope.value()) : cell(set->column_name(column)));
		}
	}
	return result;
}

connection &connection::set_query_string_buffer_size(std::size_t size)
{
	m_impl->m_query_string_buffer_size = size;
	return *this;
}

std::size_t connection::query_string_buffer_size() const
{
	return m_impl->m_query_string_buffer_size;
}

GTS_DBI_NAMESPACE_END
