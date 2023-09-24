#ifndef GTS_DBI_EXCEPTION_H
#define GTS_DBI_EXCEPTION_H

#include <gts/dbi/dbi_global.h>
#include <gts/exception.h>

namespace gts { namespace dbi
{

class error_code
{
public:
	error_code() {}
	error_code(int value, const std::string &message) :
		m_value(value), m_message(message) {}

	error_code(int value, std::string &&message) :
		m_value(value), m_message(std::move(message)) {}

	error_code(const error_code &other) :
		m_value(other.value()), m_message(other.m_message) {}

	error_code(error_code &&other) :
		m_value(other.value()), m_message(std::move(other.m_message)) {}

public:
	int value() const { return m_value; }
	std::string message() const { return m_message; }

public:
	operator bool() const {
		return value() != 0;
	}

public:
	error_code &operator=(const error_code &other)
	{
		m_value = other.m_value;
		m_message = other.m_message;
		return *this;
	}

	error_code &operator=(error_code &&other)
	{
		m_value = other.value();
		m_message = std::move(other.m_message);
		return *this;
	}

protected:
	int m_value = 0;
	std::string m_message = "success";
};

class exception : public std::exception, public error_code
{
	GTS_DISABLE_COPY_MOVE(exception)

public:
	exception(int value, const std::string &message) : error_code(value, message) {}
	exception(int value, std::string &&message) : error_code(value, std::move(message)) {}
	exception(const error_code &other) : error_code(other) {}
	exception(error_code &&other) : error_code(std::move(other)) {}

public:
	template <typename...Args>
	explicit exception(int value, fmt::format_string<Args...> fmt_value, Args&&...args) :
		error_code(value, fmt::format(fmt_value, std::forward<Args>(args)...)) {}

public:
	const char* what() const _GLIBCXX_USE_NOEXCEPT override {
		return m_message.c_str();
	}
};

}} //namespace gts::dbi

namespace fmt
{

template <>
class formatter<gts::dbi::exception> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const gts::dbi::exception &ex, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", ex.what(), ex.value());
	}
};

} //namespace fmt


#endif //GTS_DBI_EXCEPTION_H
