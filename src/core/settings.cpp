#include "settings.h"
#include <filesystem>

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
	std::filesystem::remove(m_file_name);
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

} //namespace gts
