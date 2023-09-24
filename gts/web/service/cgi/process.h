#ifndef PROCESS_H
#define PROCESS_H

#include "gts/string_list.h"
#include <unordered_map>
#include <functional>
#include <cppformat>
#include <asio.hpp>

namespace gts
{

#define _GTS_PROCESS_NOT_STRING \
	template <typename T> \
	enable_if_t< \
		not gts_is_dsame(T, std::string) and \
		not gts_is_dsame(T, char*), \
	process>&

class process_impl;

class GTSCORE_API process
{
	GTS_DISABLE_COPY_MOVE(process)
	using duration = std::chrono::milliseconds;

	template <typename T>
	using ns_type = enable_if_t<
			not gts_is_dsame(T, std::string) and
			not gts_is_same(T, const char*) and
			not gts_is_same(T, char*),
		process>;

public:
	explicit process(asio::io_context &io, const std::string &file_name = std::string());
	~process();

public:
	process &set_file(const std::string &file_name);
	std::string file() const;

public:
	template <typename...Args>
	process &add_env(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);

	template <typename T>
	ns_type<T> &add_env(const std::string &key, T &&value);

public:
	process &add_env(const std::string &key, const std::string &value);
	process &set_work_path(const std::string &path);
	bool start(const string_list &args = {});

public:
	void terminate();
	void kill();

public:
	bool is_running() const;
	bool join(int *ret_val = nullptr);

public:
	bool wait_writeable(const duration &ms, asio::error_code &error);
	bool wait_readable(const duration &ms, asio::error_code &error);
	bool wait_writeable(const duration &ms);
	bool wait_readable(const duration &ms);

public:
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout, asio::error_code &error);
	std::size_t read_some(void *buf, std::size_t size, const duration &timeout);
	std::size_t read_some(void *buf, std::size_t size, asio::error_code &error);
	std::size_t read_some(void *buf, std::size_t size);

public:
	void async_write_some(const char *buf, std::size_t size, std::function<void(asio::error_code,std::size_t)> call_back);
	void async_read_some(char *buf, std::size_t size, std::function<void(asio::error_code,std::size_t)> call_back);
	void cancel();

private:
	static void error(const asio::error_code &error, const char *func);
	friend class process_impl;
	process_impl *m_impl;
};

template <typename...Args>
process &process::add_env(const std::string &key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return add_env(key, fmt::format(value_fmt, std::forward<Args>(args)...));
}

template <typename T>
process::ns_type<T> &process::add_env(const std::string &key, T &&value) {
	return add_env(key, fmt::format("{}", std::forward<T>(value)));
}

} //namespace gts


#endif //PROCESS_H
