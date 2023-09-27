#ifndef GTS_DBI_CELL_H
#define GTS_DBI_CELL_H

#include <gts/dbi/error.h>
#include <cppoptional>
#include <cstring>

namespace gts { namespace dbi
{

class GTS_DBI_API cell
{
public:
	cell() = default;
	cell(const std::string &column_name);
	cell(const std::string &column_name, std::string data);

public:
	cell(const cell &other) = default;
	cell(cell &&other) noexcept(false);
	cell &operator=(const cell &other) = default;
	cell &operator=(cell &&other) noexcept(false);

public:
	std::string column_name() const;

public:
	bool has_value() const;
	bool is_valid() const;
	bool not_has_value() const;
	bool is_null() const;

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, bool), int)>
	bool get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, bool), int)>
	bool get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, signed char), int)>
	signed char get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, signed char), int)>
	signed char get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned char), int)>
	unsigned char get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned char), int)>
	unsigned char get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, short), int)>
	short get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, short), int)>
	short get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned short), int)>
	unsigned short get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned short), int)>
	unsigned short get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, int), int)>
	int get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, int), int)>
	int get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned int), int)>
	unsigned int get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned int), int)>
	unsigned int get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long), int)>
	long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long), int)>
	long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long), int)>
	unsigned long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long), int)>
	unsigned long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long long), int)>
	long long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long long), int)>
	long long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long long), int)>
	unsigned long long get(T default_value, int base = 10) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long long), int)>
	unsigned long long get(int base = 10) const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, float), int)>
	float get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, float), int)>
	float get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, double), int)>
	double get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, double), int)>
	double get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long double), int)>
	long double get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long double), int)>
	long double get() const noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)>
	std::string get(const T &default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)>
	std::string get(T &&default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)>
	const std::string &get() const noexcept(false);

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, std::string), int)>
	std::string &get() noexcept(false);

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_enum(T), int)>
	T get(T default_value) const noexcept;

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_enum(T), int)>
	T get() const noexcept(false);

public:
	bool to_bool(bool default_value) const noexcept;
	bool to_bool() const noexcept(false);

public:
	int8_t to_char(int8_t default_value) const noexcept;
	int8_t to_char() const noexcept(false);

public:
	uint8_t to_uchar(uint8_t default_value) const noexcept;
	uint8_t to_uchar() const noexcept(false);

public:
	int16_t to_short(int16_t default_value) const noexcept;
	int16_t to_short() const noexcept(false);

public:
	uint16_t to_ushort(uint16_t default_value) const noexcept;
	uint16_t to_ushort() const noexcept(false);

public:
	int32_t to_int(int32_t default_value) const noexcept;
	int32_t to_int() const noexcept(false);

public:
	uint32_t to_uint(uint32_t default_value) const noexcept;
	uint32_t to_uint() const noexcept(false);

public:
	int64_t to_long(int64_t default_value) const noexcept;
	int64_t to_long() const noexcept(false);

public:
	uint64_t to_ulong(uint64_t default_value) const noexcept;
	uint64_t to_ulong() const noexcept(false);

public:
	float to_float(float default_value) const noexcept;
	float to_float() const noexcept(false);

public:
	double to_double(double default_value) const noexcept;
	double to_double() const noexcept(false);

public:
	long double to_long_double(long double default_value) const noexcept;
	long double to_long_double() const noexcept(false);

public:
	std::string to_string(const std::string &default_value) const noexcept;
	std::string to_string(std::string &&default_value) const noexcept;
	const std::string &to_string() const noexcept(false);
	std::string &to_string() noexcept(false);

public:
	operator const std::string() const noexcept(false);
	operator std::string() noexcept(false);

public:
	bool operator==(const cell &other) const noexcept;
	bool operator!=(const cell &other) const noexcept;
	bool operator<(const cell &other) const noexcept(false);
	bool operator<=(const cell &other) const noexcept(false);
	bool operator>(const cell &other) const noexcept(false);
	bool operator>=(const cell &other) const noexcept(false);

private:
	bool to_bool(const std::string &str) const noexcept;

private:
	template <typename T, typename F>
	T _get(int base, F &&func, T default_value) const noexcept;

	template <typename T, typename F>
	T _get(F &&func, T default_value) const noexcept;

	template <typename T, typename F>
	T _get(int base, F &&func) const noexcept(false);

	template <typename T, typename F>
	T _get(F &&func) const noexcept(false);

private:
	std::string m_column_name = "NULL";
	cpp::optional<std::string> m_data;
};

}} //namespace gts::dbi

#include <gts/dbi/detail/cell.h>

#endif //GTS_DBI_CELL_H
