#include "string_list.h"
#include "algorithm.h"

namespace gts
{

#if 0
string_list::string_list()
{

}

string_list::string_list(const allocator_type& __a) :
	basic_string_list(__a)
{

}

string_list::string_list(size_type __n, const allocator_type& __a) :
	basic_string_list(__n, __a)
{

}

string_list::string_list(size_type __n, const value_type& __value, const allocator_type& __a) :
	basic_string_list(__n, __value, __a)
{

}

string_list::string_list(const basic_string_list& __x) :
	basic_string_list(__x)
{

}

string_list::string_list(basic_string_list&& __x) :
	basic_string_list(std::move(__x))
{

}

string_list::string_list(const basic_string_list& __x, const allocator_type& __a) :
	basic_string_list(__x, __a)
{

}

string_list::string_list(basic_string_list&& __x, const allocator_type& __a) :
	basic_string_list(std::move(__x), __a)
{

}

string_list::string_list(std::initializer_list<value_type> __l, const allocator_type& __a) :
	basic_string_list(__l, __a)
{

}

string_list::string_list(const string_list& __x) :
	basic_string_list(__x)
{

}

string_list::string_list(string_list&& __x) :
	basic_string_list(static_cast<basic_string_list&&>(__x))
{

}
#endif

std::vector<std::string> string_list::to_vector() const
{
	std::vector<std::string> result(size());
	for(std::size_t i=0; i<size(); i++)
		result[i] = operator[](i);
	return result;
}

std::vector<const char*> string_list::c_str_vector() const
{
	std::vector<const char*> result(size());
	for(std::size_t i=0; i<size(); i++)
		result[i] = operator[](i).c_str();
	return result;
}

std::string string_list::join(const std::string &splits)
{
	return string_list_join(*this, splits);
}

} //namespace gts
