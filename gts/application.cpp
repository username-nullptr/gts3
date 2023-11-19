
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

#include "application.h"
#include "private/app_info.h"
#include "gts/algorithm.h"
#include "gts/log.h"

GTS_APP_NAMESPACE_BEGIN

static std::string g_instace_name = "gts";

std::string set_instance_name(const std::string &name)
{
	g_instace_name = str_trimmed(name);
	if( g_instace_name.empty() )
		g_instace_name = "gts";
	else
	{
		for(auto &c : g_instace_name)
		{
			if( c != '/' and c != '\\' )
				continue;
			g_instace_name = "gts";
			break;
		}
	}
	return g_instace_name;
}

std::string instance_name()
{
	return g_instace_name;
}

std::string dir_path()
{
	auto file_name = file_path();
	auto index = file_name.find_last_of('/');
	if( index == std::string::npos or index == file_name.size() - 1 )
		return "./";
	return file_name.erase(index+1);
}

bool is_absolute_path(const std::string &path)
{
	return str_starts_with(path, "/")
#ifdef __unix__
			or str_starts_with(path, "~/")
#endif //unix
			;
}

using optional_string = cpp::optional<std::string>;

optional_string getenv(const std::string &key)
{
	auto value = ::getenv(key.c_str());
	return value? optional_string(value) : optional_string();
}

bool setenv(const std::string &key, const std::string &value, bool overwrite)
{
	if( ::setenv(key.c_str(), value.c_str(), overwrite) != 0 )
	{
		gts_log_error("gts::appinfo::setenv: setenv: {}. ({})", strerror(errno), errno);
		return false;
	}
	return true;
}

bool unsetenv(const std::string &key)
{
	if( ::unsetenv(key.c_str()) != 0 )
	{
		gts_log_error("gts::appinfo::unsetenv: unsetenv: {}. ({})", strerror(errno), errno);
		return false;
	}
	return true;
}

GTS_APP_NAMESPACE_END
