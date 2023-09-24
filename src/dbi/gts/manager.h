#ifndef DBI_MANAGER_H
#define DBI_MANAGER_H

#include <dbi/driver_interface.h>

namespace dbi
{

class DBI_API manager
{
public:
	static void register_driver(driver_interface *driver, bool as_default = false);
	static void unregister_driver(const std::string &name);
	static void unregister_driver(driver_interface *driver);

public:
	static driver_interface &driver(const std::string &name = "");
	static void set_default_driver(driver_interface *driver);
	static void set_default_driver(const std::string &name);
};

#define DBI_CREATE_EXECUTE(...)  dbi::manager::driver().create_execute(__VA_ARGS__)

#define DBI_CREATE_EXECUTE_BN(_driver_name, ...)  dbi::manager::driver(_driver_name).create_execute(__VA_ARGS__)

} //namespace dbi


#endif //DBI_MANAGER_H
