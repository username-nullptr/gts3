#ifndef GTS_HTTP_CONTAINER_H
#define GTS_HTTP_CONTAINER_H

#include <gts/http/value.h>
#include <unordered_map>
#include <map>

namespace gts { namespace http
{

template <typename T = http::value, GTS_TYPE_ENABLE_IF(gts_is_base_of(http::value, T), int)>
using pair = std::pair<std::string, T>;

template <typename V = http::value, typename _Compare = std::less<std::string>>
class map : public std::map<std::string, V, _Compare>
{
	static_assert(gts_is_base_of(http::value, V),
	"The template argument 'V' must be a 'gts::http::value' or derived class of 'gts::http::value'.");

public:
	using base_type = std::map<std::string, V, _Compare>;
	using base_type::map;

public:
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, const std::string &value) {
		return base_type::emplace(key, V(value));
	}
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, std::string &&value) {
		return base_type::emplace(key, V(std::move(value)));
	}

	template <typename T, typename U = http::value::not_type_t<T,int>>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, T &&value) {
		return base_type::emplace(key, V(std::forward<T>(value)));
	}

	template <typename...Args>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args) {
		return base_type::emplace(key, V(fmt_value, std::forward<Args>(args)...));
	}

public:
	V value(const std::string &key, const V &default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second;
	}

	bool value_bool(const std::string &key, bool default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_bool();
	}

	int32_t value_int(const std::string &key, int32_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_int();
	}

	uint32_t value_uint(const std::string &key, uint32_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_uint();
	}

	int64_t value_long(const std::string &key, int64_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_long();
	}

	uint64_t value_ulong(const std::string &key, uint64_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_ulong();
	}

	float value_float(const std::string &key, float default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_float();
	}

	double value_double(const std::string &key, double default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_double();
	}

	long double value_ldouble(const std::string &key, long double default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_ldouble();
	}

public:
	const V &value(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second;
	}

	bool value_bool(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_bool();
	}

	int32_t value_int(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_int();
	}

	uint32_t value_uint(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_uint();
	}

	int64_t value_long(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_long();
	}

	uint64_t value_ulong(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_ulong();
	}

	float value_float(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_float();
	}

	double value_double(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_double();
	}

	long double value_ldouble(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_ldouble();
	}

public:
	V &value(const std::string &key)
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second;
	}

public:
	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_arithmetic(T), int)>
	T value(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.template get<T>();
	}

	template <typename T, GTS_TYPE_ENABLE_IF(gts_is_arithmetic(T), int)>
	T value(const std::string &key, T default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.template get<T>();
	}

public:
	using base_type::operator[];
	const V &operator[](const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second;
	}
};

struct less_case_insensitive : std::binary_function<std::string, std::string, bool>
{
	struct nocase_compare : public std::binary_function<unsigned char, unsigned char, bool>
	{
		bool operator()(const unsigned char c1, const unsigned char c2) const {
			return tolower(c1) < tolower(c2);
		}
	};
	bool operator()(const std::string &v1, const std::string &v2) const {
		return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end(), nocase_compare());
	}
};

template <typename V = http::value>
class unordered_map : public std::unordered_map<std::string, V>
{
	static_assert(gts_is_base_of(http::value, V),
	"The template argument 'V' must be a 'gts::http::value' or derived class of 'gts::http::value'.");

public:
	using base_type = std::unordered_map<std::string, V>;
	using base_type::unordered_map;

public:
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, const std::string &value) {
		return base_type::emplace(key, V(value));
	}
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, std::string &&value) {
		return base_type::emplace(key, V(std::move(value)));
	}

	template <typename T, typename U = http::value::not_type_t<T,int>>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, T &&value) {
		return base_type::emplace(key, V(std::forward<T>(value)));
	}

	template <typename...Args>
	std::pair<typename base_type::iterator, bool> emplace(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args) {
		return base_type::emplace(key, V(fmt_value, std::forward<Args>(args)...));
	}

public:
	V value(const std::string &key, const V &default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second;
	}

	bool value_bool(const std::string &key, bool default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_bool();
	}

	int32_t value_int(const std::string &key, int32_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_int();
	}

	uint32_t value_uint(const std::string &key, uint32_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_uint();
	}

	int64_t value_long(const std::string &key, int64_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_long();
	}

	uint64_t value_ulong(const std::string &key, uint64_t default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_ulong();
	}

	float value_float(const std::string &key, float default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_float();
	}

	double value_double(const std::string &key, double default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_double();
	}

	long double value_ldouble(const std::string &key, long double default_value) const
	{
		auto it = this->find(key);
		return it == this->end()? default_value : it->second.to_ldouble();
	}

public:
	const V &value(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second;
	}

	bool value_bool(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_bool();
	}

	int32_t value_int(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_int();
	}

	uint32_t value_uint(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_uint();
	}

	int64_t value_long(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_long();
	}

	uint64_t value_ulong(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_ulong();
	}

	float value_float(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_float();
	}

	double value_double(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_double();
	}

	long double value_ldouble(const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second.to_ldouble();
	}

public:
	V &value(const std::string &key)
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second;
	}

public:
	using base_type::operator[];
	const V &operator[](const std::string &key) const
	{
		auto it = this->find(key);
		assert(it != this->end());
		return it->second;
	}
};

}} //namespace gts::http


#endif //GTS_HTTP_CONTAINER_H
