#ifndef GTS_HTTP_FMT_FORMATTER_H
#define GTS_HTTP_FMT_FORMATTER_H

#include <fmt/format.h>
#include <gts/http/types.h>

namespace fmt
{

template <>
class formatter<gts::http::status>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(gts::http::status status, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", gts::http::status_description(status));
	}
};

template <>
class formatter<gts::http::method>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(gts::http::method method, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", gts::http::method_string(method));
	}
};

} //namespace fmt


#endif //GTS_HTTP_FMT_FORMATTER_H
