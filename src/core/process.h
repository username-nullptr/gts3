#ifndef PROCESS_H
#define PROCESS_H

#include "gts/string_list.h"
#include <fmt/format.h>
#include <unordered_map>
#include <functional>
#include <asio.hpp>

namespace gts
{

#if __cplusplus < 201703L
# define _GTS_PROCESS_NOT_STRING \
	template <typename T> \
	typename std::enable_if< \
		not std::is_same<typename std::decay<T>::type, std::string>::value and \
		not std::is_same<typename std::decay<T>::type, char*>::value \
	>::type
#else //c++2017
# define _GTS_PROCESS_NOT_STRING \
	template <typename T> \
	std::enable_if_t< \
		not std::is_same_v<std::decay_t<T>, std::string> and \
		not std::is_same_v<std::decay_t<T>, char*> \
	>
#endif //c++2017

class process_private;

class GTSCORE_API process
{
	GTS_DISABLE_COPY_MOVE(process)

public:
	explicit process(asio::io_context &io, const std::string &file_name = std::string());
	~process();

public:
	void set_file(const std::string &file_name);
	std::string file() const;

public:
	template <typename...Args>
	void add_env(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);
	_GTS_PROCESS_NOT_STRING	add_env(const std::string &key, T &&value);

public:
	void add_env(const std::string &key, const std::string &value);
	void set_work_path(const std::string &path);
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
	friend class process_private;
	process_private *d_ptr;
};

template <typename...Args> inline
void process::add_env(const std::string &key, fmt::format_string<Args...> value_fmt, Args&&...args)
{
	add_env(key, fmt::format(value_fmt, std::forward<Args>(args)...));
}

_GTS_PROCESS_NOT_STRING process::add_env(const std::string &key, T &&value)
{
	add_env(key, fmt::format("{}", std::forward<T>(value)));
}

} //namespace gts


#endif //PROCESS_H
