#ifndef GTS_DBI_DRIVER_H
#define GTS_DBI_DRIVER_H

#include <gts/dbi/connection.h>

namespace gts { namespace dbi
{

class GTS_DBI_API driver
{
	GTS_DISABLE_COPY_MOVE(driver)

public:
	driver() = default;
	virtual ~driver() = 0;
	virtual bool is_valid();

public:
	virtual std::string name() = 0;
	virtual std::string description() = 0;

public:
	virtual void set_default_connect_info(const dbi::connect_info &info) = 0;
	virtual void set_default_connect_string_info(const std::string &info) = 0;

	template <typename...Args>
	void set_default_connect_string_info(fmt::format_string<Args...> fmt, Args&&...args);

public:
	virtual dbi::connect_info default_connect_info() const = 0;
	virtual std::string default_connect_string_info() const = 0;

public:
	virtual bool set_auto_commit(error_code &error, bool enable = true) noexcept = 0;
	void set_auto_commit(bool enable = true) noexcept(false);
	virtual bool auto_commit() const = 0;

public:
	virtual connection_ptr create_connection(error_code &error, const dbi::connect_info &info) noexcept = 0;
	virtual connection_ptr create_connection(error_code &error, const std::string &info) noexcept = 0;

public:
	virtual connection_ptr create_connection(error_code &error) noexcept;
	virtual connection_ptr create_connection() noexcept(false);

public:
	connection_ptr create_connection(const dbi::connect_info &info) noexcept(false);
	connection_ptr create_connection(const std::string &info) noexcept(false);
};

}} //namespace gts::dbi

#include <gts/dbi/detail/driver.h>

#endif //GTS_DBI_DRIVER_H
