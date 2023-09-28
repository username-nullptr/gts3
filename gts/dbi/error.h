#ifndef GTS_DBI_EXCEPTION_H
#define GTS_DBI_EXCEPTION_H

#include <gts/dbi/global.h>
#include <gts/exception.h>

namespace gts { namespace dbi
{

class error_code
{
public:
	error_code() = default;
	error_code(int value, std::string message);
	error_code(const error_code &other);
	error_code(error_code &&other) noexcept;

public:
	int value() const;
	std::string message() const;

public:
	operator bool() const;

public:
	error_code &operator=(const error_code &other);
	error_code &operator=(error_code &&other) noexcept;

protected:
	int m_value = 0;
	std::string m_message = "success";
};

class exception : public gts::basic_exception, public error_code
{
public:
	exception(int value, std::string message);
	exception(const error_code &other);
	exception(error_code &&other) noexcept;

public:
	template <typename...Args>
	explicit exception(int value, fmt::format_string<Args...> fmt_value, Args&&...args);

public:
	const char *what() const _GLIBCXX_USE_NOEXCEPT override;
#if GTS_CPLUSPLUS >= 201703L
private: GTS_DISABLE_COPY_MOVE(exception)
#else
public:
	exception(const exception&) = default;
	exception(exception &&other) noexcept;
#endif
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
