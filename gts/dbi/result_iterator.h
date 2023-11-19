
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

#ifndef GTS_DBI_RESULT_ITERATOR_H
#define GTS_DBI_RESULT_ITERATOR_H

#include <gts/dbi/types.h>
#include <cppoptional>
#include <memory>

GTS_DBI_NAMESPACE_BEGIN

class GTS_DBI_API result_iterator
{
	GTS_DISABLE_COPY_MOVE(result_iterator)

public:
	result_iterator() = default;
	virtual ~result_iterator() = 0;

public:
	virtual bool next() = 0;
	virtual std::size_t column_count() const = 0;
	virtual std::string column_name(std::size_t column) const = 0;

public:
	virtual cpp::optional<bool>        get_opt_bool  (std::size_t column) const = 0;
	virtual cpp::optional<int8_t>      get_opt_char  (std::size_t column) const = 0;
	virtual cpp::optional<uint8_t>     get_opt_uchar (std::size_t column) const = 0;
	virtual cpp::optional<int16_t>     get_opt_short (std::size_t column) const = 0;
	virtual cpp::optional<uint16_t>    get_opt_ushort(std::size_t column) const = 0;
	virtual cpp::optional<int32_t>     get_opt_int   (std::size_t column) const = 0;
	virtual cpp::optional<uint32_t>    get_opt_uint  (std::size_t column) const = 0;
	virtual cpp::optional<int64_t>     get_opt_long  (std::size_t column) const = 0;
	virtual cpp::optional<uint64_t>    get_opt_ulong (std::size_t column) const = 0;
	virtual cpp::optional<float>       get_opt_float (std::size_t column) const = 0;
	virtual cpp::optional<double>      get_opt_double(std::size_t column) const = 0;
	virtual cpp::optional<std::string> get_opt_string(std::size_t column, std::size_t maxlen = 1024) const = 0;
	// ... ...

public:
	virtual cpp::optional<bool>        get_opt_bool  (const std::string &column_name) const;
	virtual cpp::optional<int8_t>      get_opt_char  (const std::string &column_name) const;
	virtual cpp::optional<uint8_t>     get_opt_uchar (const std::string &column_name) const;
	virtual cpp::optional<int16_t>     get_opt_short (const std::string &column_name) const;
	virtual cpp::optional<uint16_t>    get_opt_ushort(const std::string &column_name) const;
	virtual cpp::optional<int32_t>     get_opt_int   (const std::string &column_name) const;
	virtual cpp::optional<uint32_t>    get_opt_uint  (const std::string &column_name) const;
	virtual cpp::optional<int64_t>     get_opt_long  (const std::string &column_name) const;
	virtual cpp::optional<uint64_t>    get_opt_ulong (const std::string &column_name) const;
	virtual cpp::optional<float>       get_opt_float (const std::string &column_name) const;
	virtual cpp::optional<double>      get_opt_double(const std::string &column_name) const;
	virtual cpp::optional<std::string> get_opt_string(const std::string &column_name, std::size_t maxlen = 1024) const;
	// ... ...

public:
	virtual bool        get_bool  (std::size_t column) const;
	virtual int8_t      get_char  (std::size_t column) const;
	virtual uint8_t     get_uchar (std::size_t column) const;
	virtual int16_t     get_short (std::size_t column) const;
	virtual uint16_t    get_ushort(std::size_t column) const;
	virtual int32_t     get_int   (std::size_t column) const;
	virtual uint32_t    get_uint  (std::size_t column) const;
	virtual int64_t     get_long  (std::size_t column) const;
	virtual uint64_t    get_ulong (std::size_t column) const;
	virtual float       get_float (std::size_t column) const;
	virtual double      get_double(std::size_t column) const;
	virtual std::string get_string(std::size_t column, std::size_t maxlen = 1024) const;
	// ... ...

public:
	virtual bool        get_bool  (const std::string &column_name) const;
	virtual int8_t      get_char  (const std::string &column_name) const;
	virtual uint8_t     get_uchar (const std::string &column_name) const;
	virtual int16_t     get_short (const std::string &column_name) const;
	virtual uint16_t    get_ushort(const std::string &column_name) const;
	virtual int32_t     get_int   (const std::string &column_name) const;
	virtual uint32_t    get_uint  (const std::string &column_name) const;
	virtual int64_t     get_long  (const std::string &column_name) const;
	virtual uint64_t    get_ulong (const std::string &column_name) const;
	virtual float       get_float (const std::string &column_name) const;
	virtual double      get_double(const std::string &column_name) const;
	virtual std::string get_string(const std::string &column_name, std::size_t maxlen = 1024) const;
	// ... ...

public:
	int row_id() const; //目前没用，未实现
};

using result_iterator_ptr = std::shared_ptr<result_iterator>;

GTS_DBI_NAMESPACE_END


#endif //GTS_DBI_RESULT_ITERATOR_H
