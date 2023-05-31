#ifndef GTS_LOG_BUFFER_H
#define GTS_LOG_BUFFER_H

#include <gts/log_global.h>
#include <fmt/format.h>
#include <chrono>
#include <cstdio>

namespace gts { namespace log
{

class logger;

class GTSLOG_API log_buffer
{
public:
	enum type
	{
		debug,   //stdout
		info,    //stdout
		warning, //stderr
		error,   //stderr
		fetal    //stderr-sync
	};

	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>  otime;
	struct context
	{
		otime time;
		const char *file;
		const char *func;
		int line;
	};

public:
	explicit log_buffer(type t);
	log_buffer(const log_buffer &other);
	log_buffer(log_buffer &&other);
	~log_buffer();

public:
	log_buffer &operator=(const log_buffer &other);
	log_buffer &operator=(log_buffer &&other);

public:
	template <typename T>
	log_buffer &write(T &&msg);

	template <typename T>
	log_buffer &operator<<(T &&msg);

private:
	struct data
	{
		log_buffer::type type;
		log_buffer::context context;
		std::string buffer;
	}
	*m_data;

private:
	friend class logger;
};

inline log_buffer::log_buffer(type t) : m_data(new data) {
	m_data->type = t;
}

inline log_buffer::log_buffer(const log_buffer &other) : m_data(new data) {
	operator=(other);
}

inline log_buffer::log_buffer(log_buffer &&other)
{
	m_data = other.m_data;
	other.m_data = nullptr;
}

inline log_buffer &log_buffer::operator=(const log_buffer &other)
{
	*m_data = *other.m_data;
	return *this;
}

inline log_buffer &log_buffer::operator=(log_buffer &&other)
{
	if( m_data )
		delete m_data;
	m_data = other.m_data;
	other.m_data = nullptr;
	return *this;
}

template <typename T>
inline log_buffer &log_buffer::write(T &&msg)
{
#ifndef __NO_DEBUG__
	m_data->buffer += fmt::format("{} ", std::forward<T>(msg));
#endif //__NO_DEBUG__
	return *this;
}

template <typename T>
inline log_buffer &log_buffer::operator<<(T &&msg)
{
	return write(std::forward<T>(msg));
}

}} //namespace gts::log


#endif //GTS_LOG_BUFFER_H
