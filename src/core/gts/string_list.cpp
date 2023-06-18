#include "string_list.h"
#include "algorithm.h"

namespace gts
{

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
