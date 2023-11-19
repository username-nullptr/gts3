
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

#ifndef GTS_UUID_H
#define GTS_UUID_H

#include <gts/global.h>

GTS_NAMESPACE_BEGIN

union GTSCORE_API uuid //version 4
{
public:
	uuid(const std::string &uuid);
	uuid(const uuid &other) = default;

public:
	GTS_CXX_NODISCARD("") static uuid generate();

public:
	uuid &operator=(const uuid &other) = default;
	uuid &operator=(const std::string &uuid);

public:
	bool operator==(const uuid &other) const;
	bool operator!=(const uuid &other) const;
	bool operator<(const uuid &other) const;
	bool operator>(const uuid &other) const;

public:
	// aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	GTS_CXX_NODISCARD("") std::string to_string(bool parcel = false) const;
	operator std::string() const { return to_string(); }

public:
	uint64_t _wide_integers[2];

	struct _internalData
	{
		uint32_t d0;
		uint16_t d1;
		uint16_t d2;
		uint8_t  d3[8];
	}
	_uuid;

	struct _byteRepresentation
	{
		uint8_t d0[4];
		uint8_t d1[2];
		uint8_t d2[2];
		uint8_t d3[8];
	}
	_bytes;
};

GTS_NAMESPACE_END


#endif //GTS_UUID_H
