#ifndef GTS_HTTP_VALUE_H
#define GTS_HTTP_VALUE_H

#include <gts/web_global.h>
#include <gts/algorithm.h>
#include <fmt/format.h>

namespace gts { namespace http
{

class value : public std::string
{
public:
	template <typename CT, typename T = void>
	using not_string_t = enable_if_t<not gts_is_same(decay_t<CT>, std::string) and not gts_is_same(decay_t<CT>, char*), T>;
	using base_type = std::string;

public:
	using base_type::basic_string;
	using base_type::operator+=;
	using base_type::operator=;
	using base_type::operator[];

public:
	value(const std::string &other) : base_type(other) {}
	value(const http::value &other) : base_type(other) {}
	value(std::string &&other) : base_type(std::move(other)) {}
	value(http::value &&other) : base_type(std::move(other)) {}
	value() = default;

public:
	bool        to_bool   () const { return get<bool       >(); }
	int32_t     to_int    () const { return get<int32_t    >(); }
	uint32_t    to_uint   () const { return get<uint32_t   >(); }
	int64_t     to_long   () const { return get<int64_t    >(); }
	uint64_t    to_ulong  () const { return get<uint64_t   >(); }
	float       to_float  () const { return get<float      >(); }
	double      to_double () const { return get<double     >(); }
	long double to_ldouble() const { return get<long double>(); }

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, bool), int)>
	bool get() const { return gts::stob(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, signed char), int)>
	signed char get() const { return gts::stoi8(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned char), int)>
	unsigned char get() const { return gts::stoui8(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, short), int)>
	short get() const { return gts::stoi16(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned short), int)>
	unsigned short get() const { return gts::stoui16(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, int), int)>
	int get() const { return gts::stoi32(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned int), int)>
	unsigned int get() const { return gts::stoui32(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long), int)>
	long get() const { return gts::stoi64(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long), int)>
	unsigned long get() const { return gts::stoui64(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long long), int)>
	long long get() const { return gts::stoui64(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, unsigned long long), int)>
	unsigned long long get() const { return gts::stoui64(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, float), int)>
	float get() const { return gts::stof(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, double), int)>
	double get() const { return gts::stod(*this); }

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_same(T, long double), int)>
	long double get() const { return gts::stold(*this); }

public:
	void set_value(const std::string &v) { operator=(v); }
	void set_value(std::string &&v) { operator=(std::move(v)); }

	template <typename...Args>
	void set_value(fmt::format_string<Args...> fmt_value, Args&&...args) {
		operator=(fmt::format(fmt_value, std::forward<Args>(args)...));
	}

	template <typename T, typename U = not_string_t<T,int>>
	void set_value(T &&v) {
		set_value("{}", std::forward<T>(v));
	}

public:
	template <typename...Args>
	static value from(fmt::format_string<Args...> fmt_value, Args&&...args)
	{
		value hv;
		hv.set_value(fmt::format(fmt_value, std::forward<Args>(args)...));
		return hv;
	}
	template <typename T, typename U = not_string_t<T,int>>
	static value from(T &&v)
	{
		value hv;
		set_value("{}", std::forward<T>(v));
		return hv;
	}

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_arithmetic(decay_t<T>),int)>
	value &operator=(T value)
	{
		operator=(std::to_string(value));
		return *this;
	}

public:
	value &operator=(const std::string &other) { base_type::operator=(other); return *this; }
	value &operator=(const http::value &other) { base_type::operator=(other); return *this; }
	value &operator=(std::string &&other) { base_type::operator=(std::move(other)); return *this; }
	value &operator=(http::value &&other) { base_type::operator=(std::move(other)); return *this; }
};

}} //namespace gts::http

namespace std
{

template<> struct hash<gts::http::value> : public __hash_base<size_t, gts::http::value>
{
	size_t operator()(const gts::http::value &v) const noexcept {
		return hash<std::string>()(v);
	}
};

} //namespace std


#endif //GTS_HTTP_PAIR_H
