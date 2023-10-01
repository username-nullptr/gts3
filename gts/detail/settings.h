
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

#ifndef GTS_DETAIL_SETTINGS_H
#define GTS_DETAIL_SETTINGS_H

namespace gts
{

template <typename T> inline
T settings::read(const std::string &group, const std::string &key, const T &default_value) const
{
	auto group_it = m_ini_map.find(group);
	if( group_it == m_ini_map.end() )
		return default_value;

	auto key_it = group_it->second.find(key);
	if( key_it == group_it->second.end() )
		return default_value;

	try {
		return key_it->second.as<T>();
	}
	catch(...) {}
	return default_value;
}

template <typename T> inline
settings &settings::write(const std::string &group, const std::string &key, T &&value)
{
	m_ini_map[group][key] = std::forward<T>(value);
	return *this;
}

inline settings::iterator settings::begin()
{
	return m_ini_map.begin();
}

inline settings::const_iterator settings::begin() const
{
	return m_ini_map.begin();
}

inline settings::iterator settings::end()
{
	return m_ini_map.end();
}

inline settings::const_iterator settings::end() const
{
	return m_ini_map.end();
}

inline settings::reverse_iterator settings::rbegin()
{
	return m_ini_map.rbegin();
}

inline settings::const_reverse_iterator settings::rbegin() const
{
	return m_ini_map.rbegin();
}

inline settings::reverse_iterator settings::rend()
{
	return m_ini_map.rend();
}

inline settings::const_reverse_iterator settings::rend() const
{
	return m_ini_map.rend();
}

inline settings::iterator settings::find(const std::string &group)
{
	return m_ini_map.find(group);
}

inline settings::const_iterator settings::find(const std::string &group) const
{
	return m_ini_map.find(group);
}

inline ini::IniSectionBase<std::less<std::string>> &settings::operator[](const std::string &group)
{
	return m_ini_map[group];
}

} //namespace gts


#endif //GTS_DETAIL_SETTINGS_H
