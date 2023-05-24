#ifndef LOG_OPERATOR_H
#define LOG_OPERATOR_H

#include <fmt/format.h>
#include <rttr/string_view.h>
#include <asio.hpp>
#include <thread>

namespace fmt
{

template <>
class formatter<std::thread::id>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const std::thread::id &tid, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", *reinterpret_cast<const std::thread::native_handle_type*>(&tid));
	}
};

template <>
class formatter<asio::ip::tcp::endpoint>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const asio::ip::tcp::endpoint &endpoint, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}:{}", endpoint.address().to_string(), endpoint.port());
	}
};

template <typename CharT>
class formatter<rttr::basic_string_view<CharT>>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const rttr::basic_string_view<CharT> &str, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", std::basic_string<CharT>(str));
	}
};

} //namespace fmt


#endif //LOG_OPERATOR_H
