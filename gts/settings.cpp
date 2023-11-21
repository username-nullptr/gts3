
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

#include "settings.h"
#include <cppfilesystem>

GTS_NAMESPACE_BEGIN

settings::settings(const std::string &file_name) :
	m_file_name(file_name),
	m_ini_map(file_name)
{

}

settings::~settings()
{
	if( m_delete_on_flush )
		flush();
}

settings &settings::global_instance()
{
	static settings g_instance;
	return g_instance;
}

settings &settings::set_file(const std::string &file)
{
	if( m_delete_on_flush )
		flush();
	m_file_name = file;
	return reload();
}

std::string settings::file_name() const
{
	return m_file_name;
}

settings &settings::reload()
{
	m_ini_map.load(m_file_name);
	return *this;
}

settings &settings::flush()
{
	fs::remove(m_file_name);
	m_ini_map.save(m_file_name);
	return *this;
}

settings &settings::set_delete_on_flush(bool enable)
{
	m_delete_on_flush = enable;
	return *this;
}

bool settings::delete_on_flush() const
{
	return m_delete_on_flush;
}

void settings::ini_file_check(const std::string &group, const ini_hash &sample)
{
	/*
		Check the content of the configuration file and complete the missing configuration
		items Container comparison (time complexity is too high) :
			1. Create the sample hash table
			2. Copy the sample hash table
			3. Read the actual configuration to the copy hash table
			4. Delete duplicate keys in the copy hash table
			5. Copy the remaining elements of the hash table as missing entries
			6. Save missing items
	*/
	auto tmp = sample; //O(n)
	{
		auto it = find(group); //O(1)
		if( it != end() )
		{
			for(auto &pair : it->second) //O(n)
			{
				if( sample.find(pair.first) != sample.end() ) //O(1)
					tmp.emplace(pair.first, pair.second); //O(1)
			}
		}
	}
	for(auto &pair : sample) //O(n)
	{
		if( tmp.count(pair.first) > 1 ) //O(?)
			tmp.erase(pair.first); //O(?)
	}

	int c = 0;
	for(auto &pair : tmp) //O(n)
	{
		if( pair.second.get_type() == GTS_RTTR_TYPE(int) )
			write(group, pair.first, pair.second.get_value<int>()); //O(log)

		else if( pair.second.get_type() == GTS_RTTR_TYPE(bool) )
			write(group, pair.first, pair.second.get_value<bool>()); //O(log)

		else if( pair.second.get_type() == GTS_RTTR_TYPE(std::string) )
			write(group, pair.first, pair.second.get_value<std::string>()); //O(log)

		else if( pair.second.get_type() == GTS_RTTR_TYPE(const char*) )
			write(group, pair.first, pair.second.get_value<const char*>()); //O(log)
		c++;
	}
	if( c > 0 )
		flush(); //io
}

GTS_NAMESPACE_END
