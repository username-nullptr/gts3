#ifndef DBI_FORMAT_EX_H
#define DBI_FORMAT_EX_H

#include <dbi/types.h>
#include <cppformat>

namespace fmt
{

template <>
class formatter<dbi::error_code>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const dbi::error_code &error, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", error.message(), error.value());
	}
};

template <>
class formatter<dbi::exception>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const dbi::exception &error, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", error.message(), error.value());
	}
};

template <>
class formatter<dbi::cell_data>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const dbi::cell_data &data, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", data.has_value()? data.to_string() : "NULL");
	}
};

template <>
class formatter<dbi::table_data>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const dbi::table_data &table, Context &&context) -> decltype(context.out())
	{
		std::string str;
		for(auto &rows : table)
		{
			str += "{";
			for(auto &cell : rows)
				str += "'" + (cell.has_value()? cell.to_string() : "NULL") + "'";
			str += "}";
		}
		return format_to(context.out(), "\"{}\"", str);
	}
};

} //namespace fmt


#endif //DBI_FORMAT_EX_H
