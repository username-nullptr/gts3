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
