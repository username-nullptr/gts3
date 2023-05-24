#ifndef SETTINGS_H
#define SETTINGS_H

#include "gts/gts_global.h"
#include <rttr/variant.h>
#include <inicpp.h>

namespace gts
{

class GTSCORE_API settings
{
	DISABLE_COPY(settings)

public:
	explicit settings(const std::string &file_name = std::string());
	~settings();

public:
	static settings &global_instance();

public:
	void set_file(const std::string &file);
	std::string file_name() const;

public:
	template <typename T>
	T read(const std::string &group, const std::string &key, const T &default_value = T()) const;

	template <typename T>
	void write(const std::string &group, const std::string &key, T &&value);

public:
	void reload();
	void flush();

public:
	void set_delete_on_flush(bool enable = true);
	bool delete_on_flush() const;

public:
	typedef std::unordered_multimap<std::string, rttr::variant>  ini_hash;
	static void ini_file_check(const std::string &group, const ini_hash &sample);

public:
	typedef ini::IniFile::iterator  iterator;
	typedef ini::IniFile::const_iterator  const_iterator;
	typedef ini::IniFile::reverse_iterator  reverse_iterator;
	typedef ini::IniFile::const_reverse_iterator  const_reverse_iterator;

public:
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;

public:
	iterator find(const std::string &group);
	const_iterator find(const std::string &group) const;

public:
	ini::IniSectionBase<std::less<std::string>> &operator[](const std::string &group);

private:
	std::string m_file_name;
	ini::IniFile m_ini_map;
	bool m_delete_on_flush = true;
};

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
void settings::write(const std::string &group, const std::string &key, T &&value)
{
	m_ini_map[group][key] = std::forward<T>(value);
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


#endif //SETTINGS_H
