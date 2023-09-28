#ifndef GTS_DBI_DETAIL_DRIVER_H
#define GTS_DBI_DETAIL_DRIVER_H

namespace gts { namespace dbi
{

inline driver::~driver() {}

inline bool driver::is_valid() {
	return false;
}

template <typename...Args>
void driver::set_default_connect_string_info(fmt::format_string<Args...> fmt_value, Args&&...args) {
	set_default_connect_string_info(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void driver::set_auto_commit(bool enable) noexcept(false)
{
	error_code error;
	set_auto_commit(error, enable);
	if( error )
		throw exception(error);
}

inline connection_ptr driver::create_connection(error_code &error) noexcept {
	return create_connection(error, default_connect_info());
}

inline connection_ptr driver::create_connection() noexcept(false)
{
	error_code error;
	auto res = create_connection(error, default_connect_info());
	if( error )
		throw exception(error);
	return res;
}

inline connection_ptr driver::create_connection(const dbi::connect_info &info) noexcept(false)
{
	error_code error;
	auto res = create_connection(error, info);
	if( error )
		throw exception(error);
	return res;
}

inline connection_ptr driver::create_connection(const std::string &info) noexcept(false)
{
	error_code error;
	auto res = create_connection(error, info);
	if( error )
		throw exception(error);
	return res;
}

}} //namespace gts::dbi


#endif //GTS_DBI_DETAIL_DRIVER_H
