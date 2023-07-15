#ifndef GTS_HTTP_FMT_FORMATTER_H
#define GTS_HTTP_FMT_FORMATTER_H

#include <fmt/format.h>
#include <gts/http/types.h>

namespace gts { namespace http
{

class enum_formatter
{
protected:
	enum class mode {
		number, string, str_num
	}
	m_mode = mode::number;

public:
	template <typename Context>
	inline FMT_CONSTEXPR auto parse(Context &&context) -> decltype(context.begin())
	{
		auto it = context.begin();
		if( it == context.end() or *it == '}' )
		{
			m_mode = mode::number;
			return it;
		}
		if( *it == 'n' )
		{
			m_mode = mode::number;
			++it;
		}
		else if( *it == 's' )
		{
			m_mode = mode::string;
			++it;
			if( *it == 'n' )
			{
				m_mode = mode::str_num;
				++it;
			}
		}
		if( it != context.end() and *it != '}' )
			fmt::detail::throw_format_error("Invalid gts::http enum format specifier.");
		return it;
	}
};

}} //namespace gts::http

namespace fmt
{

template <>
class formatter<gts::http::status> : public gts::http::enum_formatter
{
public:
	template <typename Context>
	inline auto format(gts::http::status status, Context &&context) -> decltype(context.out())
	{
		if( this->m_mode == mode::number )
			return format_to(context.out(), "{}", static_cast<int>(status));
		else if( this->m_mode == mode::string )
			return format_to(context.out(), "{}", gts::http::status_description(status));

		return format_to(context.out(), "{} ({})", gts::http::status_description(status), static_cast<int>(status));
	}
};

template <>
class formatter<gts::http::method> : public gts::http::enum_formatter
{
public:
	template <typename Context>
	inline auto format(gts::http::method method, Context &&context) -> decltype(context.out())
	{
		if( this->m_mode == mode::number )
			return format_to(context.out(), "{}", static_cast<int>(method));
		else if( this->m_mode == mode::string )
			return format_to(context.out(), "{}", gts::http::method_string(method));

		return format_to(context.out(), "{}({})", gts::http::method_string(method), static_cast<int>(method));
	}
};

} //namespace fmt


#endif //GTS_HTTP_FMT_FORMATTER_H
