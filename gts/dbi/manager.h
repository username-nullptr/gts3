#ifndef GTS_DBI_MANAGER_H
#define GTS_DBI_MANAGER_H

#include <gts/dbi/driver.h>

namespace gts { namespace dbi
{

class GTS_DBI_API manager
{
public:
	static void register_driver(dbi::driver *driver, bool as_default = false) noexcept(false);
	static void unregister_driver(const std::string &name);
	static void unregister_driver(dbi::driver *driver);

public:
	static dbi::driver &driver(const std::string &name = "");
	static void set_default_driver(dbi::driver *driver);
	static void set_default_driver(const std::string &name);
};

#define GTS_DBI_CREATE_EXECUTE(...)  gts::dbi::manager::driver().create_connection(__VA_ARGS__)

#define GTS_DBI_CREATE_EXECUTE_BN(_driver_name, ...)  gts::dbi::manager::driver(_driver_name).create_connection(__VA_ARGS__)

}} //namespace gts::dbi


#endif //GTS_DBI_MANAGER_H
