#ifndef GTS_LOG_H
#define GTS_LOG_H

#include <gts/log_buffer.h>
#include <gts/log_operator.h>

namespace gts { namespace log
{

#define log_debug    gts::log::logger(__FILE__, __func__, __LINE__).debug
#define log_info     gts::log::logger(__FILE__, __func__, __LINE__).info
#define log_warning  gts::log::logger(__FILE__, __func__, __LINE__).warning
#define log_error    gts::log::logger(__FILE__, __func__, __LINE__).error
#define log_fatal    gts::log::logger(__FILE__, __func__, __LINE__).fatal

class logger_impl;

class GTSLOG_API logger
{
	GTS_DISABLE_COPY_MOVE(logger)

public:
	struct context
	{
		std::string dir      = "";
		std::string category = "default";

		std::size_t max_size_one_file = 10240;
		std::size_t max_size_one_day  = 10485760;
		std::size_t max_size         = 1073741824;

		int  mask          = 4;
		int  time_category = true;
		bool async         = true;
		bool no_stdout     = false;
	};

public:
	logger(const char *file, const char *func, int line);
	~logger();

public:
	static void set_context(const context &con);
	static void set_header_breaks_aline(bool enable = true); //default false

public:
	static context get_context();
	static bool get_header_breaks_aline();

public:
	template <typename...Args>
	void debug(fmt::format_string<Args...> fmt, Args&&...args);
	void debug(const std::string &msg);
	log_buffer debug();

public:
	template <typename...Args>
	void info(fmt::format_string<Args...> fmt, Args&&...args);
	void info(const std::string &msg);
	log_buffer info();

public:
	template <typename...Args>
	void warning(fmt::format_string<Args...> fmt, Args&&...args);
	void warning(const std::string &msg);
	log_buffer warning();

public:
	template <typename...Args>
	void error(fmt::format_string<Args...> fmt, Args&&...args);
	void error(const std::string &msg);
	log_buffer error();

public:
	template <typename...Args>
	void fatal(fmt::format_string<Args...> fmt, Args&&...args);
	void fatal(const std::string &msg);

private:
	void _fatal(const std::string &msg);
	logger_impl *m_impl;
};

template <typename...Args> inline
void logger::debug(fmt::format_string<Args...> fmt, Args&&...args)
{
	debug() << fmt::format(fmt, std::forward<Args>(args)...);
}

inline void logger::debug(const std::string &msg)
{
	debug() << msg;
}

template <typename...Args> inline
void logger::info(fmt::format_string<Args...> fmt, Args&&...args)
{
	info() << fmt::format(fmt, std::forward<Args>(args)...);
}

inline void logger::info(const std::string &msg)
{
	info() << msg;
}

template <typename...Args> inline
void logger::warning(fmt::format_string<Args...> fmt, Args&&...args)
{
	warning() << fmt::format(fmt, std::forward<Args>(args)...);
}

inline void logger::warning(const std::string &msg)
{
	warning() << msg;
}

template <typename...Args> inline
void logger::error(fmt::format_string<Args...> fmt, Args&&...args)
{
	error() << fmt::format(fmt, std::forward<Args>(args)...);
}

inline void logger::error(const std::string &msg)
{
	error() << msg;
}

template <typename...Args> inline
void logger::fatal(fmt::format_string<Args...> fmt, Args&&...args)
{
	_fatal(fmt::format(fmt, std::forward<Args>(args)...));
}

inline void logger::fatal(const std::string &msg)
{
	_fatal(msg);
}

}} //namespace gts::log


#endif //GTS_LOG_H
