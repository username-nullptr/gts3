
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef GTS_FORMATTER_H
#define GTS_FORMATTER_H

#include <gts/utility.h>
#include <rttr/type>
#include <asio.hpp>

#include <cppformat>
#include <thread>
#include <atomic>
#include <memory>

GTS_NAMESPACE_BEGIN

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

template <typename T>
class formatter<std::atomic<T>> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const std::atomic<T> &n, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", n.load());
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

template <typename T>
class formatter<std::shared_ptr<T>> : public gts::no_parse_formatter
{
public:
	template <typename Context>
	inline auto format(const std::shared_ptr<T> &ptr, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}:({})", gts::type_name<T>(), reinterpret_cast<void*>(ptr.get()));
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

		else if( var.get_type() == GTS_RTTR_TYPE(int) or var.get_type().is_enumeration() )
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

GTS_NAMESPACE_END


#endif //GTS_FORMATTER_H
