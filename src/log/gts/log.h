#ifndef GTS_LOG_H
#define GTS_LOG_H

#include <gts/log_buffer.h>
#include <gts/formatter.h>

namespace gts { namespace log
{

#define gts_log_debug    gts::log::logger(__FILE__, __func__, __LINE__).debug
#define gts_log_info     gts::log::logger(__FILE__, __func__, __LINE__).info
#define gts_log_warning  gts::log::logger(__FILE__, __func__, __LINE__).warning
#define gts_log_error    gts::log::logger(__FILE__, __func__, __LINE__).error
#define gts_log_fatal    gts::log::logger(__FILE__, __func__, __LINE__).fatal

#define gts_custom_log_debug    gts::log::logger(__FILE__, __func__, __LINE__).cdebug
#define gts_custom_log_info     gts::log::logger(__FILE__, __func__, __LINE__).cinfo
#define gts_custom_log_warning  gts::log::logger(__FILE__, __func__, __LINE__).cwarning
#define gts_custom_log_error    gts::log::logger(__FILE__, __func__, __LINE__).cerror
#define gts_custom_log_fatal    gts::log::logger(__FILE__, __func__, __LINE__).cfatal

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
		std::size_t max_size          = 1073741824;

		int  mask          = 4;
		int  time_category = true;
		bool async         = false;
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
	static void wait(); //TODO...

public:
	template <typename...Args>
	log_buffer debug(fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer debug(std::string msg);
	log_buffer debug();

	template <typename...Args>
	log_buffer info(fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer info(std::string msg);
	log_buffer info();

	template <typename...Args>
	log_buffer warning(fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer warning(std::string msg);
	log_buffer warning();

	template <typename...Args>
	log_buffer error(fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer error(std::string msg);
	log_buffer error();

	template <typename...Args>
	void fatal(fmt::format_string<Args...> fmt, Args&&...args);
	void fatal(const std::string &msg);

public:
	template <typename...Args>
	log_buffer cdebug(std::string category, fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer cdebug(std::string category, std::string msg);
	log_buffer cdebug(std::string category);

	template <typename...Args>
	log_buffer cinfo(std::string category, fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer cinfo(std::string category, std::string msg);
	log_buffer cinfo(std::string category);

	template <typename...Args>
	log_buffer cwarning(std::string category, fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer cwarning(std::string category, std::string msg);
	log_buffer cwarning(std::string category);

	template <typename...Args>
	log_buffer cerror(std::string category, fmt::format_string<Args...> fmt, Args&&...args);
	log_buffer cerror(std::string category, std::string msg);
	log_buffer cerror(std::string category);

	template <typename...Args>
	void cfatal(std::string category, fmt::format_string<Args...> fmt, Args&&...args);
	void cfatal(std::string category, const std::string &msg);

public:
	static void reload();

private:
	void _fatal(const std::string &msg, std::string category = "");
	logger_impl *m_impl;
};

template <typename...Args> inline
log_buffer logger::debug(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return debug() << fmt::format(fmt_value, std::forward<Args>(args)...);
}

inline log_buffer logger::debug(std::string msg) {
	return debug() << std::move(msg);
}

template <typename...Args> inline
log_buffer logger::info(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return info() << fmt::format(fmt_value, std::forward<Args>(args)...);
}

inline log_buffer logger::info(std::string msg) {
	return info() << std::move(msg);
}

template <typename...Args> inline
log_buffer logger::warning(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return warning() << fmt::format(fmt_value, std::forward<Args>(args)...);
}

inline log_buffer logger::warning(std::string msg) {
	return warning() << std::move(msg);
}

template <typename...Args> inline
log_buffer logger::error(fmt::format_string<Args...> fmt_value, Args&&...args) {
	return error() << fmt::format(fmt_value, std::forward<Args>(args)...);
}

inline log_buffer logger::error(std::string msg) {
	return error() << std::move(msg);
}

template <typename...Args> inline
void logger::fatal(fmt::format_string<Args...> fmt_value, Args&&...args) {
	_fatal(fmt::format(fmt_value, std::forward<Args>(args)...));
}

inline void logger::fatal(const std::string &msg) {
	_fatal(msg);
}

template <typename...Args> inline
log_buffer logger::cdebug(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return cdebug(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...);
}

inline log_buffer logger::cdebug(std::string category, std::string msg) {
	return cdebug(std::move(category)) << std::move(msg);
}

template <typename...Args> inline
log_buffer logger::cinfo(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return cinfo(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...);
}

inline log_buffer logger::cinfo(std::string category, std::string msg) {
	return cinfo(std::move(category)) << std::move(msg);
}

template <typename...Args> inline
log_buffer logger::cwarning(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return cwarning(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...);
}

inline log_buffer logger::cwarning(std::string category, std::string msg) {
	return cwarning(std::move(category)) << std::move(msg);
}

template <typename...Args> inline
log_buffer logger::cerror(std::string category, fmt::format_string<Args...> fmt, Args&&...args) {
	return cerror(std::move(category)) << fmt::format(fmt, std::forward<Args>(args)...);
}

inline log_buffer logger::cerror(std::string category, std::string msg) {
	return cerror(std::move(category)) << std::move(msg);
}

template <typename...Args> inline
void logger::cfatal(std::string category, fmt::format_string<Args...> fmt_value, Args&&...args) {
	_fatal(fmt::format(fmt_value, std::forward<Args>(args)...), std::move(category));
}

inline void logger::cfatal(std::string category, const std::string &msg) {
	_fatal(msg, std::move(category));
}

}} //namespace gts::log


#endif //GTS_LOG_H
