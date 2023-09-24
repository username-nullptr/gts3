#ifndef DBI_EXCEPTION_H
#define DBI_EXCEPTION_H

#include <dbi/dbi_global.h>
#include <string>

namespace dbi
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

class exception : public error_code, public std::exception
{
	DBI_DISABLE_COPY_MOVE(exception)

public:
	exception(int value, const std::string &message) : error_code(value, message) {}
	exception(int value, std::string &&message) : error_code(value, std::move(message)) {}
	exception(const error_code &other) : error_code(other) {}
	exception(error_code &&other) : error_code(std::move(other)) {}

public:
	const char* what() const _GLIBCXX_USE_NOEXCEPT override {
		return m_message.c_str();
	}
};

} //namespace dbi


#endif //DBI_EXCEPTION_H
