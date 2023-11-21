
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

#ifndef GTS_SETTINGS_H
#define GTS_SETTINGS_H

#include <gts/value.h>
#include <rttr/variant.h>
#include <inicpp.h>

GTS_NAMESPACE_BEGIN

class GTSCORE_API settings
{
	GTS_DISABLE_COPY_MOVE(settings)

	template <typename T>
	using is_string = gts::value::is_string<T>;

	template <typename CT>
	using enum_t = enable_if_t<gts_is_enum(CT), int>;

	template <typename CT>
	using value_t = enable_if_t<is_string<CT>::value, int>;

	template <typename CT>
	using not_value_enum_t = enable_if_t<not is_string<CT>::value and not gts_is_enum(CT), int>;

public:
	explicit settings(const std::string &file_name = std::string());
	~settings();

public:
	GTS_CXX_NODISCARD("")
	static settings &global_instance();

public:
	settings &set_file(const std::string &file);
	GTS_CXX_NODISCARD("") std::string file_name() const;

public:
	template <typename T, typename U0 = not_value_enum_t<T>> GTS_CXX_NODISCARD("")
	T read(const std::string &group, const std::string &key, const T &default_value = T()) const;

	template <typename T, typename U0 = enum_t<T>, typename U1 = char> GTS_CXX_NODISCARD("")
	T read(const std::string &group, const std::string &key, T default_value = T()) const;

	template <typename T = gts::value, typename U0 = value_t<T>> GTS_CXX_NODISCARD("")
	gts::value read(const std::string &group, const std::string &key, T default_value = T()) const;

	template <typename T>
	settings &write(const std::string &group, const std::string &key, T &&value);

public:
	settings &reload();
	settings &flush();

public:
	settings &set_delete_on_flush(bool enable = true);
	GTS_CXX_NODISCARD("") bool delete_on_flush() const;

public:
	using ini_hash = std::unordered_multimap<std::string, rttr::variant>;
	void ini_file_check(const std::string &group, const ini_hash &sample);

public:
	using iterator = ini::IniFile::iterator;
	using const_iterator = ini::IniFile::const_iterator;
	using reverse_iterator = ini::IniFile::reverse_iterator;
	using const_reverse_iterator = ini::IniFile::const_reverse_iterator;

public:
	GTS_CXX_NODISCARD("") iterator begin();
	GTS_CXX_NODISCARD("") const_iterator begin() const;
	GTS_CXX_NODISCARD("") iterator end();
	GTS_CXX_NODISCARD("") const_iterator end() const;
	GTS_CXX_NODISCARD("") reverse_iterator rbegin();
	GTS_CXX_NODISCARD("") const_reverse_iterator rbegin() const;
	GTS_CXX_NODISCARD("") reverse_iterator rend();
	GTS_CXX_NODISCARD("") const_reverse_iterator rend() const;

public:
	GTS_CXX_NODISCARD("") iterator find(const std::string &group);
	GTS_CXX_NODISCARD("") const_iterator find(const std::string &group) const;

public:
	ini::IniSectionBase<std::less<std::string>> &operator[](const std::string &group);

private:
	std::string m_file_name;
	ini::IniFile m_ini_map;
	bool m_delete_on_flush = true;
};

GTS_NAMESPACE_END
#include <gts/detail/settings.h>

#endif //GTS_SETTINGS_H
