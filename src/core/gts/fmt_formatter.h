#ifndef GTS_FMT_FORMATTER_H
#define GTS_FMT_FORMATTER_H

#include <asio.hpp>
#include <gts/utility.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <rttr/type>
#include <thread>

namespace gts
{

class no_parse_formatter
{
public:
	template <typename Context>
	inline FMT_CONSTEXPR auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
};

} //namespace gts

namespace fmt
{

template <>
class formatter<std::thread::id> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const std::thread::id &tid, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", *reinterpret_cast<const std::thread::native_handle_type*>(&tid));
	}
};

template <>
class formatter<std::error_code> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const std::error_code &error, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", error.message(), error.value());
	}
};

template <>
class formatter<asio::ip::tcp::endpoint> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const asio::ip::tcp::endpoint &endpoint, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}:{}", endpoint.address().to_string(), endpoint.port());
	}
};

template <typename Fir, typename Sec>
class formatter<std::pair<Fir,Sec>> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const std::pair<Fir,Sec> &pair, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "'{}'-'{}'", pair.first, pair.second);
	}
};

template <typename CharT>
class formatter<rttr::basic_string_view<CharT>> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const rttr::basic_string_view<CharT> &str, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", std::basic_string<CharT>(str));
	}
};

template <>
class formatter<rttr::type> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const rttr::type &type, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", type.get_name().data());
	}
};

template <>
class formatter<rttr::method> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const rttr::method &method, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", method.get_name().data());
	}
};

template <>
class formatter<rttr::property> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const rttr::property &property, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", property.get_name().data());
	}
};

template <>
class formatter<rttr::variant> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const rttr::variant &var, Context &&context) -> decltype(context.out())
	{
		std::string str;
		if( var.get_type() == GTS_RTTR_TYPE(bool) )
			str = fmt::format("{}", var.get_value<bool>());

		else if( var.get_type() == GTS_RTTR_TYPE(signed char) )
			str = fmt::format("{}", var.get_value<signed char>());
		else if( var.get_type() == GTS_RTTR_TYPE(unsigned char) )
			str = fmt::format("{}", var.get_value<unsigned char>());

		else if( var.get_type() == GTS_RTTR_TYPE(short) )
			str = fmt::format("{}", var.get_value<short>());
		else if( var.get_type() == GTS_RTTR_TYPE(unsigned short) )
			str = fmt::format("{}", var.get_value<unsigned short>());

		else if( var.get_type() == GTS_RTTR_TYPE(int) )
			str = fmt::format("{}", var.get_value<int>());
		else if( var.get_type() == GTS_RTTR_TYPE(unsigned int) )
			str = fmt::format("{}", var.get_value<unsigned int>());

		else if( var.get_type() == GTS_RTTR_TYPE(long) )
			str = fmt::format("{}", var.get_value<long>());
		else if( var.get_type() == GTS_RTTR_TYPE(unsigned long) )
			str = fmt::format("{}", var.get_value<unsigned long>());

		else if( var.get_type() == GTS_RTTR_TYPE(long long) )
			str = fmt::format("{}", var.get_value<long long>());
		else if( var.get_type() == GTS_RTTR_TYPE(unsigned long long) )
			str = fmt::format("{}", var.get_value<unsigned long long>());

		else if( var.get_type() == GTS_RTTR_TYPE(float) )
			str = fmt::format("{}", var.get_value<float>());
		else if( var.get_type() == GTS_RTTR_TYPE(double) )
			str = fmt::format("{}", var.get_value<double>());
		else if( var.get_type() == GTS_RTTR_TYPE(long double) )
			str = fmt::format("{}", var.get_value<long double>());

		else if( var.get_type() == GTS_RTTR_TYPE(char*) )
			str = fmt::format("'{}'", var.get_value<char*>());
		else if( var.get_type() == GTS_RTTR_TYPE(std::string) )
			str = fmt::format("'{}'", var.get_value<std::string>());
		else if( var.get_type() == GTS_RTTR_TYPE(rttr::string_view) )
			str = fmt::format("'{}'", var.get_value<rttr::string_view>());

		else if( var.get_type().is_enumeration() )
			str = fmt::format("{}", var.get_value<int>());
		return format_to(context.out(), "rttr::variant({}:{})", var.get_type().get_name().data(), str);
	}
};

template <>
class formatter<rttr::argument> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const rttr::argument &arg, Context &&context) -> decltype(context.out()) {
		return fmt::format("{}", arg.get_value<rttr::variant>());
	}
};

template <>
class formatter<rttr::instance> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const rttr::argument &arg, Context &&context) -> decltype(context.out()) {
		return fmt::format("{}", arg.get_type());
	}
};

} //namespace fmt


#endif //GTS_FMT_FORMATTER_H
