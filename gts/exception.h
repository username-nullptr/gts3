#ifndef GTS_EXCEPTION_H
#define GTS_EXCEPTION_H

#include <gts/global.h>
#include <gts/formatter.h>
#include <exception>

namespace gts
{

class basic_exception : public std::exception {};

class exception : public basic_exception
{
public:
	explicit exception(std::string what);

	template <typename...Args>
	explicit exception(fmt::format_string<Args...> fmt_value, Args&&...args);

public:
	const char* what() const _GLIBCXX_NOTHROW override;
#if GTS_CPLUSPLUS >= 201703L
private: GTS_DISABLE_COPY_MOVE(exception)
#else
public:
	exception(const exception&) = default;
	exception(exception &&other);
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
		return format_to(context.out(), "{}", ex.what());
	}
};

} //namespace fmt

#include <gts/detail/exception.h>

#endif //GTS_EXCEPTION_H
