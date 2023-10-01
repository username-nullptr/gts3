
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

#include "json.h"
#include <gts/formatter.h>
#include <gts/utility.h>

namespace nlohmann
{

rttr::variant rjson::extract_basic_types(const rttr::type &type, json &json_value) noexcept(false)
{
	if( json_value.type() == value_t::string )
	{
		if( type == GTS_RTTR_TYPE(std::string) )
			return json_value.get<std::string>();
		throw type_error::create(1, fmt::format("字段类型不匹配 (Json:'string', Struct:'{}')", type.get_name()), nullptr);
	}
	else if( json_value.type() == value_t::boolean )
	{
		if( type == GTS_RTTR_TYPE(bool) )
			return json_value.get<bool>();
		throw type_error::create(1, fmt::format("字段类型不匹配 (Json:'bool', Struct:'{}')", type.get_name()), nullptr);
	}
	else if( json_value.type() == value_t::number_integer or json_value.type() == value_t::number_unsigned )
	{
		if( type == GTS_RTTR_TYPE(signed char) )
			return json_value.get<signed char>();
		else if( type == GTS_RTTR_TYPE(unsigned char) )
			return json_value.get<unsigned char>();

		else if( type == GTS_RTTR_TYPE(short) )
			return json_value.get<short>();
		else if( type == GTS_RTTR_TYPE(unsigned short) )
			return json_value.get<unsigned short>();

		else if( type == GTS_RTTR_TYPE(int) )
			return json_value.get<int>();
		else if( type == GTS_RTTR_TYPE(unsigned int) )
			return json_value.get<unsigned int>();

		else if( type == GTS_RTTR_TYPE(long) )
			return json_value.get<long>();
		else if( type == GTS_RTTR_TYPE(unsigned long) )
			return json_value.get<unsigned long>();

		else if( type == GTS_RTTR_TYPE(long long) )
			return json_value.get<long long>();
		else if( type == GTS_RTTR_TYPE(unsigned long long) )
			return json_value.get<unsigned long long>();

		else if( type == GTS_RTTR_TYPE(float) or type == GTS_RTTR_TYPE(double) or type == GTS_RTTR_TYPE(long double) )
			return json_value.get<uint32_t>();
		throw type_error::create(1, fmt::format("字段类型不匹配 (Json:'arithmetic', Struct:'{}')", type.get_name()), nullptr);
	}
	else if( json_value.type() == value_t::number_float )
	{
		if( not type.is_arithmetic() )
			throw type_error::create(1, fmt::format("字段类型不匹配 (Json:'float', Struct:'{}')", type.get_name()), nullptr);

		else if( type == GTS_RTTR_TYPE(float) )
			return json_value.get<float>();
		else if( type == GTS_RTTR_TYPE(double) )
			return json_value.get<double>();
		else if( type == GTS_RTTR_TYPE(long double) )
			return json_value.get<long double>();
		return json_value.get<double>();
	}
	return {};
}

rttr::variant rjson::extract_value(json &json_value, const rttr::type &type) noexcept(false)
{
	auto extracted_value = extract_basic_types(type, json_value);
	if( not extracted_value.convert(type) )
	{
		if( json_value.is_object() )
		{
			extracted_value = type.create();
			fromjson_recursively(extracted_value, json_value);
		}
	}
	return extracted_value;
}

void rjson::write_associative_view_recursively(rttr::variant_associative_view& view, json &json_array_value) noexcept(false)
{
	for(auto &json_index_value : json_array_value)
	{
		auto key_type = view.get_key_type();
		auto value_type = view.get_value_type();

		if( value_type.is_valid() )
		{
			auto key_json = json_index_value["key"];
			auto value_json = json_index_value["value"];

			auto key_var = extract_value(key_json, key_type);
			auto value_var = extract_value(value_json, value_type);

			if( key_var.is_valid() and value_var.is_valid() )
				view.insert(key_var, value_var);
		}
		else
		{
			auto extracted_value = extract_value(json_index_value, key_type);
			if( extracted_value/* and extracted_value.convert(key_type)*/ )
				view.insert(extracted_value);
		}
	}
}

void rjson::write_array_recursively(rttr::variant_sequential_view& view, json &json_array_value) noexcept(false)
{
	view.set_size(json_array_value.size());
	for(size_type i=0; i<json_array_value.size(); i++)
	{
		auto &json_index_value = json_array_value[i];
		if( json_index_value.is_array() )
		{
			auto sub_array_view = view.get_value(i).create_sequential_view();
			write_array_recursively(sub_array_view, json_index_value);
		}
		else if( json_index_value.is_object() )
		{
			auto var_tmp = view.get_value(i);
			auto wrapped_var = var_tmp.extract_wrapped_value();
			fromjson_recursively(wrapped_var, json_index_value);
			view.set_value(i, wrapped_var);
		}
		else
		{
			auto array_type = view.get_value_type();
			auto extracted_value = extract_basic_types(array_type, json_index_value);
//			if( extracted_value.convert(array_type) )
				view.set_value(i, extracted_value);
		}
	}
}

void rjson::fromjson_recursively(rttr::instance obj2, json &json_object) noexcept(false)
{
	auto obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
	const auto prop_list = obj.get_derived_type().get_properties();

	for(auto prop : prop_list)
	{
		auto it = json_object.find(prop.get_name().data());
		if( it == json_object.end() )
			continue;

		auto type = prop.get_type();
		auto &sub_json = *it;

		if( sub_json.type() == value_t::array )
		{
			rttr::variant var;
			if( type.is_sequential_container() )
			{
				var = prop.get_value(obj);
				auto view = var.create_sequential_view();
				write_array_recursively(view, sub_json);
			}
			else if( type.is_associative_container() )
			{
				var = prop.get_value(obj);
				auto view = var.create_associative_view();
				write_associative_view_recursively(view, sub_json);
			}
			prop.set_value(obj, var);
		}
		else if( sub_json.type() == value_t::object )
		{
			auto var = prop.get_value(obj);
			fromjson_recursively(var, sub_json);
			prop.set_value(obj, var);
		}
		else
		{
			auto extracted_value = extract_basic_types(prop.get_type(), sub_json);
//			if( extracted_value.convert(type) )
				prop.set_value(obj, extracted_value);
		}
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

bool rjson::write_atomic_types_to_json(const rttr::type &type, const rttr::variant &var, rttr::string_view name, json &json_object)
{
	if( type.is_arithmetic() )
	{
		if( name.empty() )
		{
			if( type == GTS_RTTR_TYPE(bool) )
				json_object.push_back(var.to_bool());
			else if (type == GTS_RTTR_TYPE(int8_t))
				json_object.push_back(var.to_int8());
			else if (type == GTS_RTTR_TYPE(int16_t))
				json_object.push_back(var.to_int16());
			else if (type == GTS_RTTR_TYPE(int32_t))
				json_object.push_back(var.to_int32());
			else if (type == GTS_RTTR_TYPE(int64_t))
				json_object.push_back(var.to_int64());
			else if (type == GTS_RTTR_TYPE(uint8_t))
				json_object.push_back(var.to_uint8());
			else if (type == GTS_RTTR_TYPE(uint16_t))
				json_object.push_back(var.to_uint16());
			else if (type == GTS_RTTR_TYPE(uint32_t))
				json_object.push_back(var.to_uint32());
			else if (type == GTS_RTTR_TYPE(uint64_t))
				json_object.push_back(var.to_uint64());
			else if (type == GTS_RTTR_TYPE(float))
				json_object.push_back(var.to_double());
			else if (type == GTS_RTTR_TYPE(double))
				json_object.push_back(var.to_double());
			else if (type == GTS_RTTR_TYPE(long double))
				json_object.push_back(var.get_value<long double>());
		}
		else
		{
			if( type == GTS_RTTR_TYPE(bool) )
				json_object.push_back({name, var.to_bool()});
			else if (type == GTS_RTTR_TYPE(int8_t))
				json_object.push_back({name, var.to_int8()});
			else if (type == GTS_RTTR_TYPE(int16_t))
				json_object.push_back({name, var.to_int16()});
			else if (type == GTS_RTTR_TYPE(int32_t))
				json_object.push_back({name, var.to_int32()});
			else if (type == GTS_RTTR_TYPE(int64_t))
				json_object.push_back({name, var.to_int64()});
			else if (type == GTS_RTTR_TYPE(uint8_t))
				json_object.push_back({name, var.to_uint8()});
			else if (type == GTS_RTTR_TYPE(uint16_t))
				json_object.push_back({name, var.to_uint16()});
			else if (type == GTS_RTTR_TYPE(uint32_t))
				json_object.push_back({name, var.to_uint32()});
			else if (type == GTS_RTTR_TYPE(uint64_t))
				json_object.push_back({name, var.to_uint64()});
			else if (type == GTS_RTTR_TYPE(float))
				json_object.push_back({name, var.to_double()});
			else if (type == GTS_RTTR_TYPE(double))
				json_object.push_back({name, var.to_double()});
			else if (type == GTS_RTTR_TYPE(long double))
				json_object.push_back({name, var.get_value<long double>()});
		}
		return true;
	}
	else if( type.is_enumeration() )
	{
		bool ok = false;
		var.to_string(&ok);
		if( ok )
			json_object.push_back({name, var.to_string()});
		else
		{
			ok = false;
			auto value = var.to_uint64(&ok);
			if( name.empty() )
			{
				if( ok )
					json_object.push_back(value);
				else
					json_object.push_back("");
			}
			else
			{
				if( ok )
					json_object.push_back({name, value});
				else
					json_object.push_back({name, ""});
			}
		}
		return true;
	}
	else if( type == GTS_RTTR_TYPE(std::string) )
	{
		if( name.empty() )
			json_object.push_back(var.to_string());
		else
			json_object.push_back({name, var.to_string()});
		return true;
	}
	return false;
}

void rjson::write_array(const rttr::variant_sequential_view &view, rttr::string_view name, json &json_object)
{
	json array(value_t::array);
	for(const auto& item : view)
	{
		if( item.is_sequential_container() )
			write_array(item.create_sequential_view(), name, array);
		else
		{
			auto wrapped_var = item.extract_wrapped_value();
			auto value_type = wrapped_var.get_type();

			if( value_type.is_arithmetic() or value_type == GTS_RTTR_TYPE(std::string) or value_type.is_enumeration() )
				write_atomic_types_to_json(value_type, wrapped_var, "", array);
			else
				to_json_recursively(wrapped_var, array);
		}
	}
	json_object.push_back({name, array});
}

void rjson::write_associative_container(const rttr::variant_associative_view &view, rttr::string_view name, json &json_object)
{
	json array(value_t::array);
	if( view.is_key_only_type() )
	{
		for(auto& pair : view)
			write_variant(array, "", pair.first);
	}
	else
	{
		for(auto& item : view)
		{
			json sub_json(value_t::object);
			write_variant(sub_json, "key", item.first);
			write_variant(sub_json, "value", item.second);
			array.push_back(sub_json);
		}
	}
	json_object.push_back({name, array});
}

void rjson::write_variant(json &json_object, rttr::string_view name, const rttr::variant &var)
{
	auto value_type = var.get_type();
	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper = wrapped_type != value_type;

	if( write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
								   is_wrapper ? var.extract_wrapped_value() : var,
								   name, json_object) )
		return ;

	else if( var.is_sequential_container() )
	{
		write_array(var.create_sequential_view(), name, json_object);
		return ;
	}
	else if( var.is_associative_container() )
	{
		write_associative_container(var.create_associative_view(), name, json_object);
		return ;
	}
	else
	{
		auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
		if( not child_props.empty() )
		{
			json sub_json(value_t::object);
			to_json_recursively(var, sub_json);

			if( name.empty() )
				json_object.push_back(sub_json);
			else
				json_object.push_back({name.data(), sub_json});
			return ;
		}
		else
		{
			bool ok = false;
			auto text = var.to_string(&ok);
			if( ok )
			{
				if( name.empty() )
					json_object.push_back(text);
				else
					json_object.push_back({name.data(), text});
				return ;
			}
		}
	}
	throw type_error::create(1, fmt::format("无效的字段类型:'{}'", value_type.get_name()), nullptr);
}

void rjson::to_json_recursively(const rttr::instance& obj2, json &json_object)
{
	auto obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
	auto prop_list = obj.get_derived_type().get_properties();

	for(auto prop : prop_list)
		write_variant(json_object, prop.get_name(), prop.get_value(obj));
}

} //namespace nlohmann
