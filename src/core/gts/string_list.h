#ifndef GTS_STRING_LIST_H
#define GTS_STRING_LIST_H

#include <gts/gts_global.h>
#include <string>
#include <vector>
#include <deque>

namespace gts
{

typedef std::deque<std::string>  basic_string_list;

class GTSCORE_API string_list : public basic_string_list
{
public:
	string_list();
	explicit string_list(const allocator_type& __a);
	explicit string_list(size_type __n, const allocator_type& __a = allocator_type());
	string_list(size_type __n, const value_type& __value, const allocator_type& __a = allocator_type());
	string_list(const basic_string_list& __x);
	string_list(basic_string_list&& __x);
	string_list(const basic_string_list& __x, const allocator_type& __a);
	string_list(basic_string_list&& __x, const allocator_type& __a);
	string_list(std::initializer_list<value_type> __l, const allocator_type& __a = allocator_type());

	template<typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>
	string_list(_InputIterator __first, _InputIterator __last, const allocator_type& __a = allocator_type()) :
		basic_string_list(__first, __last, __a) {}

public:
	string_list(const string_list& __x);
	string_list(string_list&& __x);

public:
	std::vector<std::string> to_vector() const;
	std::vector<const char*> c_str_vector() const;
};

} //namespace gts


#endif //GTS_STRING_LIST_H
