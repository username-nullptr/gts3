
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

#ifndef GTS_UTLS_DETAIL_DB_CONVERT_H
#define GTS_UTLS_DETAIL_DB_CONVERT_H

#include <gts/dbi/cell.h>

namespace gts { namespace dbi
{

namespace convert_detail
{

template <typename T>
void set_property_by_cell(T &obj, const rttr::property &property, const dbi::cell &cell) noexcept(false)
{
	if( property.get_type() == GTS_RTTR_TYPE(bool) )
		property.set_value(obj, cell.to_bool());

	else if( property.get_type() == GTS_RTTR_TYPE(std::string) )
		property.set_value(obj, cell.to_string());

	else if( property.get_type() == GTS_RTTR_TYPE(signed char) )
		property.set_value(obj, cell.get<signed char>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned char) )
		property.set_value(obj, cell.get<unsigned char>());

	else if( property.get_type() == GTS_RTTR_TYPE(short) )
		property.set_value(obj, cell.get<short>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned short) )
		property.set_value(obj, cell.get<unsigned short>());

	else if( property.get_type() == GTS_RTTR_TYPE(int) )
		property.set_value(obj, cell.get<int>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned int) )
		property.set_value(obj, cell.get<unsigned int>());

	else if( property.get_type() == GTS_RTTR_TYPE(long) )
		property.set_value(obj, cell.get<long>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned long) )
		property.set_value(obj, cell.get<unsigned long>());

	else if( property.get_type() == GTS_RTTR_TYPE(long long) )
		property.set_value(obj, cell.get<long long>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned long long) )
		property.set_value(obj, cell.get<unsigned long long>());

	else if( property.get_type() == GTS_RTTR_TYPE(float) )
		property.set_value(obj, cell.get<float>());

	else if( property.get_type() == GTS_RTTR_TYPE(double) )
		property.set_value(obj, cell.get<double>());

	else if( property.get_type() == GTS_RTTR_TYPE(long double) )
		property.set_value(obj, cell.get<long double>());

	else throw exception(-1, "db_convert: 'Invalid type of statement'.");
}

template <typename T>
void cell_to_struct_by_index(const row_vector &row, T &obj, rttr::array_range<rttr::property> &array,
							 std::size_t column) noexcept(false)
{
	assert(column == 0);
	auto it = array.begin();
	for(; it!=array.end() and column<row.size(); ++it, ++column)
		set_property_by_cell(obj, *it, row[column]);
}

template <typename T>
void cell_to_struct_by_index(const row_vector &row, T &obj, rttr::array_range<rttr::property> &array,
							 std::size_t column, std::size_t index) noexcept(false)
{
	if( index >= array.size() or column >= row.size() )
		throw exception(-2, "db_convert: 'Invalid column id'.");
	else
	{
		auto it = std::next(array.begin(), index);
		set_property_by_cell(obj, *it, row[column]);
	}
}

template <typename T, typename...Tail>
void cell_to_struct_by_index(const row_vector &row, T &obj, rttr::array_range<rttr::property> &array,
							 std::size_t column, std::size_t index, Tail...indexes) noexcept(false)
{
	cell_to_struct_by_index(row, obj, array, column++, index);
	cell_to_struct_by_index(row, obj, array, column++, indexes...);
}

} //namespace convert_detail

/*----------------------------------------------------------------------------------------------------------------------------------------*/

template <std::size_t...indexes, typename T>
void cell_to_struct_by_index(const row_vector &row, T &obj) noexcept(false)
{
	auto array = rttr::type::get<T>().get_properties();
	convert_detail::cell_to_struct_by_index(row, obj, array, 0, indexes...);
}

template <std::size_t...indexes, typename T>
T cell_to_struct_by_index(const row_vector &row) noexcept(false)
{
	T obj;
	cell_to_struct_by_index<indexes...>(row, obj);
	return obj;
}

template <std::size_t...indexes, typename T>
void table_to_struct_by_index(const table_data &table, std::vector<T> &obj) noexcept(false)
{
	obj.reserve(64);
	for(auto &row : table)
	{
		obj.emplace_back();
		cell_to_struct_by_index<indexes...>(row, obj.back());
	}
}

template <typename T, std::size_t...indexes>
std::vector<T> table_to_struct_by_index(const table_data &table)
{
	std::vector<T> result;
	table_to_struct_by_index<indexes...>(table, result);
	return result;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

template <typename T>
void cell_to_struct_by_name(const row_vector &row, T &obj) noexcept(false)
{
	auto type = rttr::type::get<T>();
	for(auto &cell : row)
	{
		auto property = type.get_property(cell.column_name());
		if( not property.is_valid() )
			throw exception(-1, "db_convert: 'Column not found (structure error)'.");
		else
			convert_detail::set_property_by_cell(obj, property, cell);
	}
}

template <typename T>
T cell_to_struct_by_name(const row_vector &row) noexcept(false)
{
	T obj;
	cell_to_struct_by_name(row, obj);
	return obj;
}

template <typename T>
void table_to_struct_by_name(const table_data &table, std::vector<T> &obj)
{
	obj.reserve(64);
	for(auto &row : table)
	{
		obj.emplace_back();
		cell_to_struct_by_name(row, obj.back());
	}
}

template <typename T>
std::vector<T> table_to_struct_by_name(const table_data &table)
{
	std::vector<T> result;
	table_to_struct_by_name(table, result);
	return result;
}

}} //namespace gts::dbi


#endif //GTS_UTLS_CONVERT_DETAIL_DB_H
