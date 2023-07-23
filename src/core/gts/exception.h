#ifndef GTS_EXCEPTION_H
#define GTS_EXCEPTION_H

#include <gts/gts_global.h>
#include <gts/fmt_formatter.h>
#include <exception>

namespace gts
{

class exception : public std::exception
{
	GTS_DISABLE_COPY_MOVE(exception)

public:
	explicit exception(const std::string &what) : m_what(what) {}
	explicit exception(std::string &&what) : m_what(std::move(what)) {}

	template <typename...Args>
	explicit exception(fmt::format_string<Args...> fmt_value, Args&&...args) :
		m_what(fmt::format(fmt_value, std::forward<Args>(args)...)) {}

public:
	const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
		return m_what.c_str();
	}

private:
	std::string m_what;
};

} //namespace gts


#endif //GTS_EXCEPTION_H
