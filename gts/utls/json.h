#ifndef GTS_JSON_H
#define GTS_JSON_H

#include <gts/utls/registration.h>
#include <nlohmann/json.hpp>

namespace nlohmann
{

class rjson : public json
{
public:
	using json::basic_json;
	rjson(const json &other) : json(other) {}
	rjson(json &&other) : json(std::move(other)) {}
	rjson() {}

public:
	rjson &operator=(const json &other)
	{
		json::operator=(other);
		return *this;
	}
	rjson &operator=(json &&other)
	{
		json::operator=(std::move(other));
		return *this;
	}

public:
	template <typename T>
	void to_struct(T &t) {
		fromjson_recursively(t, *this);
	}

	template <typename T>
	T to_struct()
	{
		T obj;
		to_struct(obj);
		return obj;
	}

	template <typename T>
	static void to_struct(const json &j, T &t) {
		fromjson_recursively(t, j);
	}

	template <typename T>
	static T to_struct(const json &j)
	{
		T obj;
		to_struct(j, obj);
		return obj;
	}

public:
	static rjson parse(const std::string &str) {
		return json::parse(str);
	}

	template <typename T>
	static void parse_to_struct(const std::string &str, T &obj) {
		parse(str).to_struct(obj);
	}

	template <typename T>
	static T parse_to_struct(const std::string &str)
	{
		T obj;
		parse_to_struct(str, obj);
		return obj;
	}

public:
	template <typename T>
	static rjson from_struct(const T &obj)
	{
		rjson j(value_t::object);
		to_json_recursively(obj, j);
		return j;
	}

private:
	static rttr::variant extract_basic_types(const rttr::type&, json&) noexcept(false);
	static rttr::variant extract_value(json&, const rttr::type&) noexcept(false);
	static void write_associative_view_recursively(rttr::variant_associative_view&, json&) noexcept(false);
	static void write_array_recursively(rttr::variant_sequential_view&, json&) noexcept(false);
	static void fromjson_recursively(rttr::instance, json&) noexcept(false);

private:
	static bool write_atomic_types_to_json(const rttr::type&, const rttr::variant&, rttr::string_view, json&);
	static void write_array(const rttr::variant_sequential_view&, rttr::string_view, json&);
	static void write_associative_container(const rttr::variant_associative_view&, rttr::string_view, json&);
	static void write_variant(json&, rttr::string_view, const rttr::variant&);
	static void to_json_recursively(const rttr::instance&, json&);
};

} //namespace nlohmann

using njson = nlohmann::rjson;


#endif //GTS_JSON_H
