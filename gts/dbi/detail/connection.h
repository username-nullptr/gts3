#ifndef GTS_DBI_DETAIL_CONNECTION_H
#define GTS_DBI_DETAIL_CONNECTION_H

namespace gts { namespace dbi
{

inline void connection::connect(const connect_info &info) noexcept(false)
{
	error_code error;
	connect(info, error);
	if( error )
		throw exception(error);
}

inline void connection::connect(const std::string &info) noexcept(false)
{
	error_code error;
	connect(info, error);
	if( error )
		throw exception(error);
}

inline void connection::disconnect() noexcept(false)
{
	error_code error;
	disconnect(error);
	if( error )
		throw exception(error);
}

inline connection &connection::operator<<(const std::string &statement) noexcept {
	return prepare_statement(statement);
}

template <typename...Args>
connection &connection::prepare(fmt::format_string<Args...> fmt_value, Args&&...args) noexcept {
	return prepare_statement(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void connection::execute() noexcept(false)
{
	error_code error;
	execute(error);
	if( error )
		throw exception(error);
}

inline void connection::roll_back() noexcept(false)
{
	error_code error;
	roll_back(error);
	if( error )
		throw exception(error);
}

inline void connection::commit() noexcept(false)
{
	error_code error;
	commit(error);
	if( error )
		throw exception(error);
}

template <typename...Args>
inline result_iterator_ptr connection::create_query(error_code &error, fmt::format_string<Args...> fmt_value, Args&&...args) noexcept {
	return create_query_work(fmt::format(fmt_value, std::forward<Args>(args)...), error);
}

template <typename...Args>
inline table_data connection::query(error_code &error, fmt::format_string<Args...> fmt_value, Args&&...args) noexcept {
	return prepare_query(fmt::format(fmt_value, std::forward<Args>(args)...), error);
}

inline table_data connection::query(error_code &error, const std::string &sql) noexcept {
	return prepare_query(sql, error);
}

template <typename...Args>
inline result_iterator_ptr connection::create_query(fmt::format_string<Args...> fmt_value, Args&&...args) noexcept(false)
{
	error_code error;
	auto res = create_query_work(fmt::format(fmt_value, std::forward<Args>(args)...), error);
	if( error )
		throw exception(error);
	return res;
}

template <typename...Args>
inline table_data connection::query(fmt::format_string<Args...> fmt_value, Args&&...args) noexcept(false)
{
	error_code error;
	auto res = prepare_query(fmt::format(fmt_value, std::forward<Args>(args)...), error);
	if( error )
		throw exception(error);
	return res;
}

inline table_data connection::query(const std::string &sql) noexcept(false)
{
	error_code error;
	auto res = prepare_query(sql, error);
	if( error )
		throw exception(error);
	return res;
}

inline void connection::set_auto_commit(bool enable) noexcept(false)
{
	error_code error;
	set_auto_commit(error, enable);
	if( error )
		throw exception(error);
}

}} //namespace gts::dbi


#endif //GTS_DBI_DETAIL_CONNECTION_H
