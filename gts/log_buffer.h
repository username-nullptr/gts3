#ifndef GTS_LOG_BUFFER_H
#define GTS_LOG_BUFFER_H

#include <gts/global.h>
#include <cppformat>
#include <chrono>
#include <cstdio>

namespace gts
{

class logger;

class GTSCORE_API log_buffer
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

	using otime = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
	struct context
	{
		otime time;
		std::string category;
		const char *file;
		const char *func;
		int line;

		context() = default;
		context(const context &other) = default;
		context &operator=(const context &other) = default;
		context(context &&other);
		context &operator=(context &&other);
	};

public:
	explicit log_buffer(type t);
	log_buffer(const log_buffer &other);
	log_buffer(log_buffer &&other) noexcept;
	~log_buffer();

public:
	log_buffer &operator=(const log_buffer &other);
	log_buffer &operator=(log_buffer &&other) noexcept;

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

} //namespace gts

#include <gts/detail/log_buffer.h>

#endif //GTS_LOG_BUFFER_H
