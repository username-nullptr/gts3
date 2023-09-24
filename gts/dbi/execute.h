#ifndef GTS_DBI_EXECUTE_H
#define GTS_DBI_EXECUTE_H

#include <gts/dbi/result_set.h>
#include <gts/dbi/formatter.h>

namespace gts { namespace dbi
{

class driver;
class execute_impl;

class GTS_DBI_API connection
{
	GTS_DISABLE_COPY_MOVE(connection)

public:
	connection();
	virtual ~connection();

public:
	virtual bool connect(const connect_info &info, error_code &error) = 0;
	virtual bool connect(const std::string &info, error_code &error) = 0;
	void connect(const connect_info &info);
	void connect(const std::string &info);

public:
	virtual void disconnect();
	void disconnect(error_code &error);

public:
	template <typename...Args>
	connection &prepare(fmt::format_string<Args...> fmt, Args&&...args);
	connection &operator<<(const std::string &statement);

public:
	virtual void set_binary(std::size_t column, const binary &data) = 0;
	// ... ...

public:
	virtual bool execute(error_code &error) = 0;
	void execute();

public:
    virtual bool roll_back(error_code &error) = 0;
    void roll_back();

public:
	virtual bool commit(error_code &error) = 0;
	void commit();

public:
	template <typename...Args>
	result_iterator_ptr create_query(error_code &error, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename...Args>
	table_data query(error_code &error, fmt::format_string<Args...> fmt, Args&&...args);
	table_data query(error_code &error, const std::string &sql);

	template <typename...Args>
	result_iterator_ptr create_query(fmt::format_string<Args...> fmt, Args&&...args);

	template <typename...Args>
	table_data query(fmt::format_string<Args...> fmt, Args&&...args);
	table_data query(const std::string &sql);

	virtual table_data prepare_query(const std::string &statement, error_code &error);

public:
	virtual bool set_auto_commit(error_code &error, bool enable = true) = 0;
	void set_auto_commit(bool enable = true);
	virtual bool auto_commit() const = 0;

public:
	virtual connection &set_query_string_buffer_size(std::size_t size);
    virtual std::size_t query_string_buffer_size() const;
	virtual dbi::driver &driver() const = 0;

protected: // 准备SQL语句（子类实现）
	virtual connection &prepare_statement(const std::string &statement) = 0;
	virtual result_iterator_ptr create_query_work(const std::string &statement, error_code &error) = 0;

private:
	execute_impl *m_impl;
};

using execute_interface_ptr = std::shared_ptr<connection>;

inline void connection::connect(const connect_info &info)
{
	error_code error;
	connect(info, error);
	if( error )
		throw exception(error);
}

inline void connection::connect(const std::string &info)
{
	error_code error;
	connect(info, error);
	if( error )
		throw exception(error);
}

inline void connection::disconnect()
{
	error_code error;
	disconnect(error);
}

inline connection &connection::operator<<(const std::string &statement) {
	return prepare_statement(statement);
}

template <typename...Args>
connection &connection::prepare(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return prepare_statement(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void connection::execute()
{
	error_code error;
	execute(error);
	if( error )
		throw exception(error);
}

inline void connection::roll_back()
{
	error_code error;
	roll_back(error);
	if( error )
		throw exception(error);
}

inline void connection::commit()
{
	error_code error;
	commit(error);
	if( error )
		throw exception(error);
}

inline void connection::set_auto_commit(bool enable)
{
	error_code error;
	set_auto_commit(error, enable);
	if( error )
		throw exception(error);
}

template <typename...Args>
inline result_iterator_ptr connection::create_query(error_code &error, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return create_query_work(fmt::format(fmt_value, std::forward<Args>(args)...), error);
}

template <typename...Args>
inline table_data connection::query(error_code &error, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return prepare_query(fmt::format(fmt_value, std::forward<Args>(args)...), error);
}

inline table_data connection::query(error_code &error, const std::string &sql) {
    return prepare_query(sql, error);
}

template <typename...Args>
inline result_iterator_ptr connection::create_query(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	error_code error;
	auto res = create_query_work(fmt::format(fmt_value, std::forward<Args>(args)...), error);
	if( error )
		throw exception(error);
	return res;
}

template <typename...Args>
inline table_data connection::query(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	error_code error;
	auto res = prepare_query(fmt::format(fmt_value, std::forward<Args>(args)...), error);
	if( error )
		throw exception(error);
	return res;
}

inline table_data connection::query(const std::string &sql)
{
    error_code error;
    auto res = prepare_query(sql, error);
    if( error )
        throw exception(error);
    return res;
}

}} //namespace gts::dbi


#endif //GTS_DBI_EXECUTE_H
