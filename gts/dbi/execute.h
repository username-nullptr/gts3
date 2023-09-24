#ifndef DBI_EXECUTE_INTERFACE_H
#define DBI_EXECUTE_INTERFACE_H

#include <dbi/result_set.h>
#include <dbi/format_ex.h>

namespace dbi
{

class driver;
class execute_interface_impl;

// SQL执行 / 数据库连接
class DBI_API execute_interface
{
	DBI_DISABLE_COPY_MOVE(execute_interface)

public:
	execute_interface()
	virtual ~execute_interface();

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
	execute_interface &prepare(fmt::format_string<Args...> fmt, Args&&...args);
	execute_interface &operator<<(const std::string &statement);

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
    virtual execute_interface &set_query_string_buffer_size(std::size_t size);
    virtual std::size_t query_string_buffer_size() const;
	virtual driver &driver() const = 0;

protected: // 准备SQL语句（子类实现）
	virtual execute_interface &prepare_statement(const std::string &statement) = 0;
	virtual result_iterator_ptr create_query_work(const std::string &statement, error_code &error) = 0;

private:
    execute_interface_impl *m_impl;
};

using execute_interface_ptr = std::shared_ptr<execute_interface>;

inline void execute_interface::connect(const connect_info &info)
{
	error_code error;
	connect(info, error);
	if( error )
		throw exception(error);
}

inline void execute_interface::connect(const std::string &info)
{
	error_code error;
	connect(info, error);
	if( error )
		throw exception(error);
}

inline void execute_interface::disconnect()
{
	error_code error;
	disconnect(error);
}

inline execute_interface &execute_interface::operator<<(const std::string &statement) {
	return prepare_statement(statement);
}

template <typename...Args>
execute_interface &execute_interface::prepare(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return prepare_statement(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void execute_interface::execute()
{
	error_code error;
	execute(error);
	if( error )
		throw exception(error);
}

inline void execute_interface::roll_back()
{
	error_code error;
	roll_back(error);
	if( error )
		throw exception(error);
}

inline void execute_interface::commit()
{
	error_code error;
	commit(error);
	if( error )
		throw exception(error);
}

inline void execute_interface::set_auto_commit(bool enable)
{
	error_code error;
	set_auto_commit(error, enable);
	if( error )
		throw exception(error);
}

template <typename...Args>
inline result_iterator_ptr execute_interface::create_query(error_code &error, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return create_query_work(fmt::format(fmt_value, std::forward<Args>(args)...), error);
}

template <typename...Args>
inline table_data execute_interface::query(error_code &error, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return prepare_query(fmt::format(fmt_value, std::forward<Args>(args)...), error);
}

inline table_data execute_interface::query(error_code &error, const std::string &sql) {
    return prepare_query(sql, error);
}

template <typename...Args>
inline result_iterator_ptr execute_interface::create_query(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	error_code error;
	auto res = create_query_work(fmt::format(fmt_value, std::forward<Args>(args)...), error);
	if( error )
		throw exception(error);
	return res;
}

template <typename...Args>
inline table_data execute_interface::query(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	error_code error;
	auto res = prepare_query(fmt::format(fmt_value, std::forward<Args>(args)...), error);
	if( error )
		throw exception(error);
	return res;
}

inline table_data execute_interface::query(const std::string &sql)
{
    error_code error;
    auto res = prepare_query(sql, error);
    if( error )
        throw exception(error);
    return res;
}

} //namespace dbi


#endif //DBI_EXECUTE_INTERFACE_H
