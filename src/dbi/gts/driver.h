#ifndef GTS_DBI_DRIVER_H
#define GTS_DBI_DRIVER_H

#include <gts/dbi/execute.h>

namespace dbi
{

class DBI_API driver
{
	DBI_DISABLE_COPY_MOVE(driver_interface)

public:
	driver() {}
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
	virtual bool set_auto_commit(error_code &error, bool enable = true) = 0;
	void set_auto_commit(bool enable = true);
	virtual bool auto_commit() const = 0;

public:
	virtual execute_interface_ptr create_execute(error_code &error, const dbi::connect_info &info) = 0;
	virtual execute_interface_ptr create_execute(error_code &error, const std::string &info) = 0;

public:
	virtual execute_interface_ptr create_execute(error_code &error);
	virtual execute_interface_ptr create_execute();

public:
	execute_interface_ptr create_execute(const dbi::connect_info &info);
	execute_interface_ptr create_execute(const std::string &info);
};

inline driver::~driver() {}

inline bool driver::is_valid() {
	return false;
}

template <typename...Args>
void driver::set_default_connect_string_info(fmt::format_string<Args...> fmt_value, Args&&...args) {
	set_default_connect_string_info(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void driver::set_auto_commit(bool enable)
{
	error_code error;
	set_auto_commit(error, enable);
	if( error )
		throw exception(error);
}

inline execute_interface_ptr driver::create_execute(error_code &error) {
	return create_execute(error, default_connect_info());
}

inline execute_interface_ptr driver::create_execute()
{
	error_code error;
	auto res = create_execute(error, default_connect_info());
	if( error )
		throw exception(error);
	return res;
}

inline execute_interface_ptr driver::create_execute(const dbi::connect_info &info)
{
	error_code error;
	auto res = create_execute(error, info);
	if( error )
		throw exception(error);
	return res;
}

inline execute_interface_ptr driver::create_execute(const std::string &info)
{
	error_code error;
	auto res = create_execute(error, info);
	if( error )
		throw exception(error);
	return res;
}

} //namespace dbi


#endif //GTS_DBI_DRIVER_H
