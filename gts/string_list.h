#ifndef GTS_STRING_LIST_H
#define GTS_STRING_LIST_H

#include <gts/gts_global.h>
#include <string>
#include <vector>
#include <deque>

namespace gts
{

using basic_string_list = std::deque<std::string>;

class GTSCORE_API string_list : public basic_string_list
{
public:
	using basic_string_list::deque;

public:
	std::vector<std::string> to_vector() const;
	std::vector<const char*> c_str_vector() const;

public:
	std::string join(const std::string &splits = " ");
};

} //namespace gts


#endif //GTS_STRING_LIST_H
