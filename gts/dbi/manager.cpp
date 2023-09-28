#include "manager.h"
#include "gts/log.h"
#include <cassert>
#include <map>

namespace gts { namespace dbi
{

static std::map<std::string, driver*>  g_driver_map;

static driver *g_default_driver = nullptr;

void manager::register_driver(dbi::driver *driver, bool as_default) noexcept(false)
{
	assert(driver);
	auto pair = g_driver_map.emplace(driver->name(), driver);
	if( pair.second == false )
	{
		gts_log_error("db_interface::manager::register_driver: multiple registration.");
		if( pair.first->second == driver )
			throw exception(-1, "driver multiple registration.");
	}
	else if( g_driver_map.size() == 1 or as_default )
		g_default_driver = driver;
}

void manager::unregister_driver(const std::string &name)
{
	auto it = g_driver_map.find(name);
	if( it == g_driver_map.end() )
		return ;

	auto driver = it->second;

	delete it->second;
	g_driver_map.erase(it);

	if( driver == g_default_driver )
		g_default_driver = g_driver_map.empty()? nullptr : g_driver_map.begin()->second;
}

void manager::unregister_driver(dbi::driver *driver)
{
	assert(driver);
	unregister_driver(driver->name());
}

static class GTS_DECL_EXPORT invalid_driver : public driver
{
public:
	std::string name() override { return ""; }
	std::string description() override { return ""; }
	void set_default_connect_info(const dbi::connect_info&) override {}
	void set_default_connect_string_info(const std::string&) override {}
	dbi::connect_info default_connect_info() const override { return {}; }
	std::string default_connect_string_info() const override { return ""; }
	bool set_auto_commit(error_code&, bool = true) noexcept override { return false; }
	bool auto_commit() const override { return false; }

	connection_ptr create_connection(error_code &error, const dbi::connect_info&) noexcept override
	{
		error = error_code(-200, "Invalid driver.");
		return connection_ptr(nullptr);
	}
	connection_ptr create_connection(error_code &error, const std::string&) noexcept override
	{
		error = error_code(-200, "Invalid driver.");
		return connection_ptr(nullptr);
	}
}
g_invalid_driver;

dbi::driver &manager::driver(const std::string &name)
{
	if( name.empty() )
	{
		if( g_default_driver )
			return *g_default_driver;

		gts_log_error("db_interface::manager::driver: No available drivers.");
		return g_invalid_driver;
	}
	auto it = g_driver_map.find(name);
	if( it == g_driver_map.end() )
	{
		gts_log_error("db_interface::manager::driver: No available drivers.");
		return g_invalid_driver;
	}
	return *it->second;
}

void manager::set_default_driver(dbi::driver *driver)
{
	assert(driver);
	auto it = g_driver_map.find(driver->name());
	if( it == g_driver_map.end() )
		g_driver_map.emplace(driver->name(), driver);
	g_default_driver = driver;
}

void manager::set_default_driver(const std::string &name)
{
	auto it = g_driver_map.find(name);
	if( it == g_driver_map.end() )
		gts_log_error("db_interface::manager::set_default_driver: dirver not registration.");
	else
		g_default_driver = it->second;
}

}} //namespace gts::dbi
