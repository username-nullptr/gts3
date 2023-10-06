
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

#include "uuid.h"
#include <random>
#include <cstdio>
#include <cinttypes>

namespace gts
{

uuid::uuid(const std::string &uuid)
{
	if( uuid.size() == 38 ) //aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	{
		sscanf(uuid.c_str(), "%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8,
			   &_uuid.d0, &_uuid.d1, &_uuid.d2, &_uuid.d3[0], &_uuid.d3[1], &_uuid.d3[2], &_uuid.d3[3], &_uuid.d3[4], &_uuid.d3[5], &_uuid.d3[6], &_uuid.d3[7]);
	}
	else if( uuid.size() == 40 ) //{aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee}
	{
		sscanf(uuid.c_str(), "{%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "}",
			   &_uuid.d0, &_uuid.d1, &_uuid.d2, &_uuid.d3[0], &_uuid.d3[1], &_uuid.d3[2], &_uuid.d3[3], &_uuid.d3[4], &_uuid.d3[5], &_uuid.d3[6], &_uuid.d3[7]);
	}
	else
		memset(this, 0, sizeof(uuid));
}

uuid uuid::generate()
{
#if defined(__APPLE__) || defined(__clang__)
	std::random_device rd;
	std::mt19937_64 gen(rd());
#else
	thread_local std::random_device rd;
	thread_local std::mt19937_64 gen(rd());
#endif
	std::uniform_int_distribution<uint64_t> dis64;
	uuid obj("");

	obj._wide_integers[0] = dis64(gen);
	obj._wide_integers[1] = dis64(gen);

	obj._bytes.d3[0] = (obj._bytes.d3[0] & 0x3F) | static_cast<uint8_t>(0x80);
	obj._bytes.d2[1] = (obj._bytes.d2[1] & 0x0F) | static_cast<uint8_t>(0x40);
	return obj;
}

uuid &uuid::operator=(const std::string &uuid)
{
	if( uuid.size() == 38 ) //aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	{
		sscanf(uuid.c_str(), "%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8,
			   &_uuid.d0, &_uuid.d1, &_uuid.d2, &_uuid.d3[0], &_uuid.d3[1], &_uuid.d3[2], &_uuid.d3[3], &_uuid.d3[4], &_uuid.d3[5], &_uuid.d3[6], &_uuid.d3[7]);
	}
	else if( uuid.size() == 40 ) //{aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee}
	{
		sscanf(uuid.c_str(), "{%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "}",
			   &_uuid.d0, &_uuid.d1, &_uuid.d2, &_uuid.d3[0], &_uuid.d3[1], &_uuid.d3[2], &_uuid.d3[3], &_uuid.d3[4], &_uuid.d3[5], &_uuid.d3[6], &_uuid.d3[7]);
	}
	return *this;
}

bool uuid::operator==(const uuid &other) const
{
	return memcmp(&other, this, sizeof(uuid)) == 0;
}

bool uuid::operator!=(const uuid &other) const
{
	return !operator==(other);
}

bool uuid::operator<(const uuid &other) const
{
	return memcmp(this, &other, sizeof(uuid)) < 0;
}

bool uuid::operator>(const uuid &other) const
{
	return memcmp(this, &other, sizeof(uuid)) > 0;
}

std::string uuid::to_string(bool parcel) const
{
	char buffer[41] = ""; //aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	if( parcel )
	{
		sprintf(buffer, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
				_uuid.d0, _uuid.d1, _uuid.d2, _uuid.d3[0], _uuid.d3[1], _uuid.d3[2], _uuid.d3[3], _uuid.d3[4], _uuid.d3[5], _uuid.d3[6], _uuid.d3[7]);
	}
	else
	{
		sprintf(buffer, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				_uuid.d0, _uuid.d1, _uuid.d2, _uuid.d3[0], _uuid.d3[1], _uuid.d3[2], _uuid.d3[3], _uuid.d3[4], _uuid.d3[5], _uuid.d3[6], _uuid.d3[7]);
	}
	return buffer;
}

} //namespace gts
