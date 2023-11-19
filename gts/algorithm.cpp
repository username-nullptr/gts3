
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

#include "algorithm.h"
#include <algorithm>

GTS_NAMESPACE_BEGIN

int8_t stoi8(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtol(str.c_str(), &end, base);
}

uint8_t stoui8(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtoul(str.c_str(), &end, base);
}

int16_t stoi16(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtol(str.c_str(), &end, base);
}

uint16_t stoui16(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtoul(str.c_str(), &end, base);
}

int32_t stoi32(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtol(str.c_str(), &end, base);
}

uint32_t stoui32(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtoul(str.c_str(), &end, base);
}

int64_t stoi64(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtoll(str.c_str(), &end, base);
}

uint64_t stoui64(const std::string &str, int base)
{
	char *end = nullptr;
	return std::strtoull(str.c_str(), &end, base);
}

float stof(const std::string &str)
{
	char *end = nullptr;
	return std::strtof(str.c_str(), &end);
}

double stod(const std::string &str)
{
	char *end = nullptr;
	return std::strtod(str.c_str(), &end);
}

double stold(const std::string &str)
{
	char *end = nullptr;
	return std::strtold(str.c_str(), &end);
}

bool stob(const std::string &str)
{
#ifdef _WINDOWS
		if( stricmp(str.c_str(), "true") == 0 )
			return true;
		else if( stricmp(str.c_str(), "false") == 0 )
			return false;
#else
		if( str.size() == 4 and strncasecmp(str.c_str(), "true", 4) == 0 )
			return true;
		else if( str.size() == 5 and strncasecmp(str.c_str(), "false", 5) == 0 )
			return false;
#endif
		return stoi8(str);
}

string_list str_split(const std::string &str, const std::string &splits, bool ignore_empty)
{
	string_list result;
	if( str.empty() )
		return result;

	auto strs = str + splits;
	auto pos = strs.find(splits);
	int step = splits.size();

	while( pos != std::string::npos )
	{
		auto tmp = strs.substr(0, pos);
		if( not tmp.empty() or not ignore_empty )
			result.emplace_back(std::move(tmp));

		strs = strs.substr(pos + step, strs.size());
		pos = strs.find(splits);
	}
	return result;
}

std::string str_list_join(const basic_string_list &list, const std::string &splits)
{
	std::string result;
	if( list.empty() )
		return result;

	std::size_t i = 0;
	for(; i<list.size()-1; i++)
		result += list[i] + splits;

	result += list[i];
	return result;
}

#if GTS_CPLUSPLUS >= 202002L

bool str_starts_with(const std::string &str, const std::string &prefix)
{
	return str.starts_with(prefix);
}

bool str_ends_with(const std::string &str, const std::string &suffix)
{
	return str.ends_with(suffix);
}

#else //<c++20

bool str_starts_with(const std::string &str, const std::string &prefix)
{
	if( str.size() < prefix.size() )
		return false;

	for(std::size_t i=0; i<prefix.size(); i++)
	{
		if( str[i] != prefix[i] )
			return false;
	}
	return true;
}

bool str_ends_with(const std::string &str, const std::string &suffix)
{
	if( str.size() < suffix.size() )
		return false;

	for(std::size_t i=suffix.size(); i>0; i--)
	{
		if( str[str.size()-i] != suffix[i-1] )
			return false;
	}
	return true;
}

#endif //c++20

std::string str_to_lower(const std::string &str)
{
	auto tmp = str;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
	return tmp;
}

std::string str_to_upper(const std::string &str)
{
	auto tmp = str;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
	return tmp;
}

std::size_t str_replace(std::string &str, const std::string &_old, const std::string &_new)
{
	std::size_t sum = 0;
	std::size_t old_pos = 0;
	while( str.find(_old, old_pos) != std::string::npos )
	{
		int start = str.find(_old, old_pos);
		str.replace(start, _old.size(), _new);
		old_pos = start + _new.size();
	}
	return sum;
}

std::string str_trimmed(const std::string &str)
{
	std::string result;

	std::size_t left = 0;
	while( left < str.size() )
	{
		if( str[left] >= 1 and str[left] <= 32 )
			left++;
		else
			break;
	}
	if( left >= str.size() )
		return result;

	int right = static_cast<int>(str.size() - 1);
	while( right >= 0 )
	{
		if( str[right] >= 1 and str[right] <= 32 )
			right--;
		else
			break;
	}
	if( right < 0 )
		return result;

	result = str.substr(0, right + 1);
	result = result.substr(left);
	return result;
}

std::string str_remove(const std::string &str, const std::string &find)
{
	std::string res = str;
	str_replace(res, find, "");
	return res;
}

std::string str_remove(const std::string &str, char find)
{
	std::string res = str;
	auto it = std::remove(res.begin(), res.end(), find);
	if( it != res.end() )
		res.erase(it, res.end());
	return res;
}

std::string from_percent_encoding(const std::string &str)
{
	std::string result;
	if( str.empty() )
		return result;

	result = str;
	char *data = const_cast<char*>(result.c_str());
	const char *inputPtr = result.c_str();

	std::size_t i = 0;
	std::size_t len = str.size();
	std::size_t outlen = 0;
	int a, b;
	char c;

	while( i < len )
	{
		c = inputPtr[i];
		if( c == '%' and i + 2 < len )
		{
			a = inputPtr[++i];
			b = inputPtr[++i];

			if( a >= '0' and a <= '9' )
				a -= '0';

			else if( a >= 'a' and a <= 'f' )
				a = a - 'a' + 10;

			else if( a >= 'A' && a <= 'F' )
				a = a - 'A' + 10;

			if( b >= '0' and b <= '9' )
				b -= '0';

			else if( b >= 'a' and b <= 'f' )
				b  = b - 'a' + 10;

			else if( b >= 'A' and b <= 'F' )
				b  = b - 'A' + 10;

			*data++ = static_cast<char>((a << 4) | b);
		}
		else
			*data++ = c;

		++i;
		++outlen;
	}

	if( outlen != len )
		result = result.substr(0, outlen);
	return result;
}

std::string file_name(const std::string &file_name)
{
	auto pos = file_name.rfind("/");
	if( pos == std::string::npos )
		return file_name;
	return file_name.substr(pos + 1);
}

std::string file_path(const std::string &file_name)
{
	auto pos = file_name.rfind("/");
	if( pos == std::string::npos )
		return ".";
	return file_name.substr(0, pos + 1);
}

GTS_NAMESPACE_END
