#include "settings.h"
#include <cppfilesystem>

namespace gts
{

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

void settings::set_file(const std::string &file)
{
	flush();
	m_file_name = file;
	reload();
}

std::string settings::file_name() const
{
	return m_file_name;
}

void settings::reload()
{
	m_ini_map.load(m_file_name);
}

void settings::flush()
{
	fs::remove(m_file_name);
	m_ini_map.save(m_file_name);
}

void settings::set_delete_on_flush(bool enable)
{
	m_delete_on_flush = enable;
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
	auto &_settings = gts::settings::global_instance();

	auto tmp = sample; //O(n)
	{
		auto it = _settings.find(group); //O(1)
		if( it != _settings.end() )
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
			_settings.write(group, pair.first, pair.second.get_value<int>()); //O(log)

		else if( pair.second.get_type() == GTS_RTTR_TYPE(bool) )
			_settings.write(group, pair.first, pair.second.get_value<bool>()); //O(log)

		else if( pair.second.get_type() == GTS_RTTR_TYPE(std::string) )
			_settings.write(group, pair.first, pair.second.get_value<std::string>()); //O(log)

		else if( pair.second.get_type() == GTS_RTTR_TYPE(const char*) )
			_settings.write(group, pair.first, pair.second.get_value<const char*>()); //O(log)
		c++;
	}
	if( c > 0 )
		_settings.flush(); //io
}

} //namespace gts
