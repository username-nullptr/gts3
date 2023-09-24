#ifndef GTS_DBI_CELL_H
#define GTS_DBI_CELL_H

#include <dbi/utility.h>
#include <dbi/error.h>
#include <cppoptional>
#include <cstring>

#ifndef _MSVC
# include <cxxabi.h>
#endif //VS

namespace dbi
{

class GTS_DBI_API cell
{
public:
	cell() = default;
	explicit cell(const std::string &column_name) :
		m_column_name(column_name) {}

	cell(const std::string &column_name, const std::string &data) :
		m_column_name(column_name), m_data(data) {}

	cell(const std::string &column_name, std::string &&data) :
		m_column_name(column_name), m_data(std::move(data)) {}

public:
	cell(const cell &other) = default;
	cell(cell &&other) :
        m_column_name(std::move(other.m_column_name)), m_data(std::move(other.m_data)) {}

	cell &operator=(const cell &other) = default;
	cell &operator=(cell &&other)
    {
        m_column_name = std::move(other.m_column_name);
        m_data = std::move(other.m_data);
        return *this;
    }

public:
	std::string column_name() const {
		return m_column_name;
	}

public:
	bool has_value() const {
		return m_data.has_value();
	}
	bool is_valid() const {
		return has_value();
	}
	bool not_has_value() const {
		return not has_value();
	}
	bool is_null() const {
		return not_has_value();
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, bool), int)>
	bool get(T default_value) const
	{
		if( has_value() )
			return to_bool(get<std::string>(default_value? "true" : "false"));
		return default_value;
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, bool), int)>
	bool get() const
	{
		if( has_value() )
			return to_bool(get<std::string>());
		throw exception(-1, "dbi::cell_data::get<bool>: Optional is empty.");
		return false;
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, signed char), int)>
	signed char get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtol, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, signed char), int)>
	signed char get(int base = 10) const {
		return _get<T>(base, std::strtol);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned char), int)>
	unsigned char get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtol, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned char), int)>
	unsigned char get(int base = 10) const {
		return _get<T>(base, std::strtol);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, short), int)>
	short get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtol, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, short), int)>
	short get(int base = 10) const {
		return _get<T>(base, std::strtol);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned short), int)>
	unsigned short get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtol, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned short), int)>
	unsigned short get(int base = 10) const {
		return _get<T>(base, std::strtol);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, int), int)>
	int get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtol, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, int), int)>
	int get(int base = 10) const {
		return _get<T>(base, std::strtol);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned int), int)>
	unsigned int get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtoul, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned int), int)>
	unsigned int get(int base = 10) const {
		return _get<T>(base, std::strtoul);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, long), int)>
	long get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtol, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, long), int)>
	long get(int base = 10) const {
		return _get<T>(base, std::strtol);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned long), int)>
	unsigned long get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtoul, default_value);
	}

	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned long), int)>
	unsigned long get(int base = 10) const {
		return _get<T>(base, std::strtoul);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, long long), int)>
	long long get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtoll, default_value);
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, long long), int)>
	long long get(int base = 10) const {
		return _get<T>(base, std::strtoll);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned long long), int)>
	unsigned long long get(T default_value, int base = 10) const {
		return _get<T>(base, std::strtoull, default_value);
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, unsigned long long), int)>
	unsigned long long get(int base = 10) const {
		return _get<T>(base, std::strtoull);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, float), int)>
	float get(T default_value) const {
		return _get<T>(std::strtof, default_value);
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, float), int)>
	float get() const {
		return _get<T>(std::strtof);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, double), int)>
	double get(T default_value) const {
		return _get<T>(std::strtod, default_value);
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, double), int)>
	double get() const {
		return _get<T>(std::strtod);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, long double), int)>
	long double get(T default_value) const {
		return _get<T>(std::strtold, default_value);
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, long double), int)>
	long double get() const {
		return _get<T>(std::strtold);
	}

public:
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, std::string), int)>
	std::string get(const T &default_value) const {
		return m_data.value_or(default_value);
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, std::string), int)>
	std::string get(T &&default_value) const {
		return m_data.value_or(std::move(default_value));
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, std::string), int)>
	const std::string &get() const
	{
		if( has_value() )
			return m_data.value();
		throw exception(-1, "dbi::cell_data::get<std::string>: Optional is empty.");
	}
	template <typename T, DBI_TYPE_ENABLE_IF(dbi_is_same(T, std::string), int)>
	std::string &get()
	{
		if( has_value() )
			return m_data.value();
		throw exception(-1, "dbi::cell_data::get<std::string>: Optional is empty.");
	}

public:
	bool to_bool(bool default_value) const {
		return get<bool>(default_value);
	}
	bool to_bool() const
	{
		if( has_value() )
			return to_bool(get<std::string>());
		throw exception(-1, "dbi::cell_data::to_bool: Optional is empty.");
		return 0;
	}

public:
	int8_t to_char(int8_t default_value) const {
		return get<int8_t>(default_value);
	}
	int8_t to_char() const
	{
		if( has_value() )
			return get<int8_t>();
		throw exception(-1, "dbi::cell_data::to_char: Optional is empty.");
		return 0;
	}

public:
	uint8_t to_uchar(uint8_t default_value) const {
		return get<uint8_t>(default_value);
	}
	uint8_t to_uchar() const
	{
		if( has_value() )
			return get<uint8_t>();
		throw exception(-1, "dbi::cell_data::to_uchar: Optional is empty.");
		return 0;
	}

public:
	int16_t to_short(int16_t default_value) const {
		return get<int16_t>(default_value);
	}
	int16_t to_short() const
	{
		if( has_value() )
			return get<int16_t>();
		throw exception(-1, "dbi::cell_data::to_short: Optional is empty.");
		return 0;
	}

public:
	uint16_t to_ushort(uint16_t default_value) const {
		return get<uint16_t>(default_value);
	}
	uint16_t to_ushort() const
	{
		if( has_value() )
			return get<uint16_t>();
		throw exception(-1, "dbi::cell_data::to_ushort: Optional is empty.");
		return 0;
	}

public:
	int32_t to_int(int32_t default_value) const {
		return get<int32_t>(default_value);
	}
	int32_t to_int() const
	{
		if( has_value() )
			return get<int32_t>();
		throw exception(-1, "dbi::cell_data::to_int: Optional is empty.");
		return 0;
	}

public:
	uint32_t to_uint(uint32_t default_value) const {
		return get<uint32_t>(default_value);
	}
	uint32_t to_uint() const
	{
		if( has_value() )
			return get<uint32_t>();
		throw exception(-1, "dbi::cell_data::to_uint: Optional is empty.");
		return 0;
	}

public:
	int64_t to_long(int64_t default_value) const {
		return get<int64_t>(default_value);
	}
	int64_t to_long() const
	{
		if( has_value() )
			return get<int64_t>();
		throw exception(-1, "dbi::cell_data::to_long: Optional is empty.");
		return 0;
	}

public:
	uint64_t to_ulong(uint64_t default_value) const {
		return get<uint64_t>(default_value);
	}
	uint64_t to_ulong() const
	{
		if( has_value() )
			return get<uint64_t>();
		throw exception(-1, "dbi::cell_data::to_ulong: Optional is empty.");
		return 0;
	}

public:
	float to_float(float default_value) const {
		return get<float>(default_value);
	}
	float to_float() const
	{
		if( has_value() )
			return get<float>();
		throw exception(-1, "dbi::cell_data::to_float: Optional is empty.");
		return 0.0;
	}

public:
	double to_double(double default_value) const {
		return get<double>(default_value);
	}
	double to_double() const
	{
		if( has_value() )
			return get<double>();
		throw exception(-1, "dbi::cell_data::to_double: Optional is empty.");
		return 0.0;
	}

public:
	long double to_long_double(long double default_value) const {
		return get<long double>(default_value);
	}
	long double to_long_double() const
	{
		if( has_value() )
			return get<long double>();
		throw exception(-1, "dbi::cell_data::to_long_double: Optional is empty.");
		return 0.0;
	}

public:
	std::string to_string(const std::string &default_value) const {
		return get<std::string>(default_value);
	}
	std::string to_string(std::string &&default_value) const {
		return get<std::string>(std::move(default_value));
	}
	const std::string &to_string() const
	{
		if( has_value() )
			return get<std::string>();
		throw exception(-1, "dbi::cell_data::to_string: Optional is empty.");
	}
	std::string &to_string()
	{
		if( has_value() )
			return get<std::string>();
		throw exception(-1, "dbi::cell_data::to_string: Optional is empty.");
	}
	operator const std::string() const {
		return to_string();
	}
	operator std::string() {
		return to_string();
	}

public:
	bool operator==(const cell &other) const {
        return has_value() and other.has_value() and m_data.value() == other.m_data.value();
    }
	bool operator!=(const cell &other) const {
        return not operator==(other);
    }
	bool operator<(const cell &other) const {
        return to_string() < other.to_string();
    }
	bool operator<=(const cell &other) const {
        return to_string() <= other.to_string();
    }
	bool operator>(const cell &other) const {
        return to_string() > other.to_string();
    }
	bool operator>=(const cell &other) const {
        return to_string() >= other.to_string();
    }

private:
	bool to_bool(const std::string &str) const
	{
#ifdef __unix
		if( str.size() == 4 and strncasecmp(str.c_str(), "true", 4) )
			return true;
		else if( str.size() == 5 and strncasecmp(str.c_str(), "false", 5) )
			return false;
#else
		if( stricmp(str.c_str(), "true") )
			return true;
		else if( stricmp(str.c_str(), "false") )
			return false;
#endif
		try {
			return std::stoi(str);
		} catch(...) {}
		return false;
	}

private:
	template <typename T, typename F>
	T _get(int base, F &&func, T default_value) const
	{
		if( has_value() )
		{
			char *end = nullptr;
			return func(m_data.value().c_str(), &end, base);
		}
		return default_value;
	}

	template <typename T, typename F>
	T _get(F &&func, T default_value) const
	{
		if( has_value() )
		{
			char *end = nullptr;
			return func(m_data.value().c_str(), &end);
		}
		return default_value;
	}

	template <typename T, typename F>
	T _get(int base, F &&func) const
	{
		if( has_value() )
		{
			char *end = nullptr;
			return func(m_data.value().c_str(), &end, base);
		}
		throw exception(-1, std::string("dbi::cell_data::get<") +
				#ifdef _MSVC
						typeid(T).name
				#else
						abi::__cxa_demangle(typeid(T).name(), 0,0,0)
				#endif
						+ ">: Optional is empty.");
		return T();
	}

	template <typename T, typename F>
	T _get(F &&func) const
	{
		if( has_value() )
		{
			char *end = nullptr;
			return func(m_data.value().c_str(), &end);
		}
		throw exception(-1, std::string("dbi::cell_data::get<") +
				#ifdef _MSVC
						typeid(T).name
				#else
						abi::__cxa_demangle(typeid(T).name(), 0,0,0)
				#endif
						+ ">: Optional is empty.");
		return T();
	}

private:
	std::string m_column_name = "NULL";
	cpp::optional<std::string> m_data;
};

inline bool operator==(const cell &cell, const std::string &str) {
    return cell.has_value() and cell.to_string() == str;
}
inline bool operator!=(const cell &cell, const std::string &str) {
    return not operator(cell, str);
}
inline bool operator>(const cell &cell, const std::string &str) {
    return cell.to_string() > str;
}
inline bool operator<(const cell &cell, const std::string &str) {
    return cell.to_string() < str;
}
inline bool operator>=(const cell &cell, const std::string &str) {
    return cell.to_string() >= str;
}
inline bool operator<=(const cell &cell, const std::string &str) {
    return cell.to_string() <= str;
}

inline bool operator==(const std::string &str, const cell &cell) {
    return cell.has_value() and cell.to_string() == str;
}
inline bool operator!=(const std::string &str, const cell &cell) {
    return not operator(cell, str);
}
inline bool operator>(const std::string &str, const cell &cell) {
    return str > cell.to_string();
}
inline bool operator<(const std::string &str, const cell &cell) {
    return str < cell.to_string();
}
inline bool operator>=(const std::string &str, const cell &cell) {
    return str >= cell.to_string();
}
inline bool operator<=(const std::string &str, const cell &cell) {
    return str <= cell.to_string();
}

inline std::string operator+(const cell &cell, const std::string &str) {
    return cell.has_value() ? cell.to_string() + str : str;
}
inline std::string operator+(const std::string &str, const cell &cell) {
    return cell.has_value() ? str + cell.to_string() : str;
}

} //namespace dbi


#endif //GTS_DBI_CELL_H
