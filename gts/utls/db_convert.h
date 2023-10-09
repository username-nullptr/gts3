
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

#ifndef GTS_UTLS_DB_CONVERT_H
#define GTS_UTLS_DB_CONVERT_H

#include <gts/utls/registration.h>

namespace gts { namespace dbi
{

template <std::size_t...indexes, typename T>
void cell_to_struct_by_index(const row_vector &row, T &obj) noexcept(false);

template <std::size_t...indexes, typename T>
T cell_to_struct_by_index(const row_vector &row) noexcept(false);

template <std::size_t...indexes, typename T>
void table_to_struct_by_index(const table_data &table, std::vector<T> &obj) noexcept(false);

template <typename T, std::size_t...indexes>
std::vector<T> table_to_struct_by_index(const table_data &table);

/*----------------------------------------------------------------------------------------------------------------------------------------*/

template <typename T>
void cell_to_struct_by_name(const row_vector &row, T &obj) noexcept(false);

template <typename T>
T cell_to_struct_by_name(const row_vector &row) noexcept(false);

template <typename T>
void table_to_struct_by_name(const table_data &table, std::vector<T> &obj);

template <typename T>
std::vector<T> table_to_struct_by_name(const table_data &table);

}} //namespace gts::dbi

#include <gts/utls/detail/db_convert.h>

#endif //GTS_UTLS_DB_CONVERT_H
