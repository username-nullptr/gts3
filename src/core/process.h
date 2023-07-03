#ifndef PROCESS_H
#define PROCESS_H

#include "gts/string_list.h"
#include <fmt/format.h>
#include <unordered_map>
#include <functional>
#include <asio.hpp>

namespace gts
{

#define _GTS_PROCESS_NOT_STRING \
	template <typename T> \
	enable_if_t< \
		not gts_is_same(decay_t<T>, std::string) and \
		not gts_is_same(decay_t<T>, char*), \
	process>&

class process_impl;

class GTSCORE_API process
{
	GTS_DISABLE_COPY_MOVE(process)

public:
	explicit process(asio::io_context &io, const std::string &file_name = std::string());
	~process();

public:
	process &set_file(const std::string &file_name);
	std::string file() const;

public:
	template <typename...Args>
	process &add_env(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);
	_GTS_PROCESS_NOT_STRING	add_env(const std::string &key, T &&value);

public:
	process &add_env(const std::string &key, const std::string &value);
	process &set_work_path(const std::string &path);
	bool start(const string_list &args = {});

public:
	void terminate();
	void kill();

public:
	bool is_running() const;
	void join();

public:
	int write(const char *buf, int size, int timeout = -1);
	int read(const char *buf, int size, int timeout = -1);

public:
	void async_write(const char *buf, int size, std::function<void(asio::error_code,std::size_t)> call_back);
	void async_read(char *buf, int size, std::function<void(asio::error_code,std::size_t)> call_back);
	void cancel();

private:
	friend class process_impl;
	process_impl *m_impl;
};

template <typename...Args> inline
process &process::add_env(const std::string &key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return add_env(key, fmt::format(value_fmt, std::forward<Args>(args)...));
}

_GTS_PROCESS_NOT_STRING process::add_env(const std::string &key, T &&value) {
	return add_env(key, fmt::format("{}", std::forward<T>(value)));
}

} //namespace gts


#endif //PROCESS_H
