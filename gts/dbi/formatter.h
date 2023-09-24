#ifndef GTS_DBI_FORMATTER_H
#define GTS_DBI_FORMATTER_H

#include <gts/dbi/types.h>
#include <cppformat>

namespace fmt
{

template <>
class formatter<gts::dbi::error_code>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const gts::dbi::error_code &error, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{} ({})", error.message(), error.value());
	}
};

template <>
class formatter<gts::dbi::cell>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const gts::dbi::cell &data, Context &&context) -> decltype(context.out()) {
		return format_to(context.out(), "{}", data.has_value()? data.to_string() : "NULL");
	}
};

template <>
class formatter<gts::dbi::table_data>
{
public:
	template <typename Context>
	inline constexpr auto parse(Context &&context) -> decltype(context.begin()) {
		return context.begin();
	}
	template <typename Context>
	inline auto format(const gts::dbi::table_data &table, Context &&context) -> decltype(context.out())
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


#endif //GTS_DBI_FORMATTER_H
