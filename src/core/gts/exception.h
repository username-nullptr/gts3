#ifndef GTS_EXCEPTION_H
#define GTS_EXCEPTION_H

#include <gts/gts_global.h>
#include <gts/formatter.h>
#include <exception>

namespace gts
{

class exception : public std::exception
{
public:
	explicit exception(const std::string &what) : m_what(what) {}
	explicit exception(std::string &&what) : m_what(std::move(what)) {}

	template <typename...Args>
	explicit exception(fmt::format_string<Args...> fmt_value, Args&&...args) :
		m_what(fmt::format(fmt_value, std::forward<Args>(args)...)) {}

public:
	const char* what() const _GLIBCXX_NOTHROW override {
		return m_what.c_str();
	}
#if __cplusplus >= 201703L
private: GTS_DISABLE_COPY_MOVE(exception)
#else
public:
	exception(const exception&) = default;
	exception(exception &&other): m_what(std::move(other.m_what)) {}
#endif
private:
	std::string m_what;
};

} //namespace gts

namespace fmt
{

template <>
class formatter<gts::exception> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const gts::exception &ex, Context &&context) -> decltype(context.out()) {
		return fmt::format("{}", ex.what());
	}
};

} //namespace fmt


#endif //GTS_EXCEPTION_H
