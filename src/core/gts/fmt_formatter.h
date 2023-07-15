#ifndef GTS_FMT_FORMATTER_H
#define GTS_FMT_FORMATTER_H

#include <asio.hpp>
#include <fmt/format.h>
#include <rttr/string_view.h>
#include <thread>

namespace fmt
{

template <>
class formatter<std::thread::id>
{
public:
	template <typename Context>
	inline FMT_CONSTEXPR auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const std::thread::id &tid, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", *reinterpret_cast<const std::thread::native_handle_type*>(&tid));
	}
};

template <typename CharT>
class formatter<rttr::basic_string_view<CharT>>
{
public:
	template <typename Context>
	inline FMT_CONSTEXPR auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const rttr::basic_string_view<CharT> &str, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", std::basic_string<CharT>(str));
	}
};

template <>
class formatter<asio::error_code>
{
public:
	template <typename Context>
	inline FMT_CONSTEXPR auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const asio::error_code &error, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", error.message(), error.value());
	}
};

template <>
class formatter<asio::ip::tcp::endpoint>
{
public:
	template <typename Context>
	inline FMT_CONSTEXPR auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const asio::ip::tcp::endpoint &endpoint, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}:{}", endpoint.address().to_string(), endpoint.port());
	}
};

} //namespace fmt


#endif //GTS_FMT_FORMATTER_H
