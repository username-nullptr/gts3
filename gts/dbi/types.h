
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

#ifndef GTS_DBI_TYPES_H
#define GTS_DBI_TYPES_H

#include <gts/dbi/cell.h>
#include <vector>
#include <map>

GTS_DBI_NAMESPACE_BEGIN

struct connect_info
{
	std::string driver; //odbc using
	std::string ip;
	uint16_t port;
	std::string user;
	std::string passwd;
};

using binary = std::vector<char>;

class row_vector : public std::vector<cell>
{
public:
	using std::vector<cell>::vector;
	using std::vector<cell>::operator[];

public:
	reference operator[](const std::string &column_name) noexcept(false)
	{
		for(auto &cell : *this)
		{
			if( cell.column_name() == column_name )
				return cell;
		}
		throw exception(-1, "row_vector::operator[](std::string): Invalid column name: '{}'.", column_name);
	}
};

using table_data = std::vector<row_vector>;

GTS_DBI_NAMESPACE_END


#endif //GTS_DBI_TYPES_H
