#ifndef GTS_SETTINGS_H
#define GTS_SETTINGS_H

#include <gts/global.h>
#include <rttr/variant.h>
#include <inicpp.h>

namespace gts
{

class GTSCORE_API settings
{
	GTS_DISABLE_COPY_MOVE(settings)

public:
	explicit settings(const std::string &file_name = std::string());
	~settings();

public:
	static settings &global_instance();

public:
	settings &set_file(const std::string &file);
	std::string file_name() const;

public:
	template <typename T>
	T read(const std::string &group, const std::string &key, const T &default_value = T()) const;

	template <typename T>
	settings &write(const std::string &group, const std::string &key, T &&value);

public:
	settings &reload();
	settings &flush();

public:
	settings &set_delete_on_flush(bool enable = true);
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

} //namespace gts

#include <gts/detail/settings.h>

#endif //GTS_SETTINGS_H
