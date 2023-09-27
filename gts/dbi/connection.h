#ifndef GTS_DBI_CONNECTION_H
#define GTS_DBI_CONNECTION_H

#include <gts/dbi/result_iterator.h>
#include <gts/dbi/formatter.h>

namespace gts { namespace dbi
{

class driver;
class connection_impl;

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
	virtual void disconnect(error_code &error) = 0;
	void disconnect();

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

protected:
	virtual connection &prepare_statement(const std::string &statement) = 0;
	virtual result_iterator_ptr create_query_work(const std::string &statement, error_code &error) = 0;

private:
	connection_impl *m_impl;
};

using connection_ptr = std::shared_ptr<connection>;

}} //namespace gts::dbi

#include <gts/dbi/detail/connection.h>

#endif //GTS_DBI_CONNECTION_H
