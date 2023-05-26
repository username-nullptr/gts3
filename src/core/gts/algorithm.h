#ifndef GTS_ALGORITHM_H
#define GTS_ALGORITHM_H

#include <gts/gts_global.h>
#include <rttr/variant.h>
#include <unordered_map>

namespace gts
{

GTSCORE_API string_list string_split(const std::string &str, const std::string &splits, bool ignore_empty = true);
GTSCORE_API std::string string_list_join(const string_list &vector, const std::string &splits = " ");

GTSCORE_API bool starts_with(const std::string &str, const std::string &prefix);
GTSCORE_API bool ends_with(const std::string &str, const std::string &suffix);

GTSCORE_API std::string to_lower(const std::string &str);
GTSCORE_API std::string to_upper(const std::string &str);

GTSCORE_API std::string trimmed(const std::string &str);

GTSCORE_API std::string from_percent_encoding(const std::string &str);

inline string_list string_split(const std::string &str, char splits, bool ignore_empty = true)
{ return string_split(str, std::string(1,splits), ignore_empty); }

GTSCORE_API std::string file_name(const std::string &file_name);
GTSCORE_API std::string file_path(const std::string &file_name);

} //namespace gts


#endif //GTS_ALGORITHM_H
