#ifndef GTS_ALGORITHM_H
#define GTS_ALGORITHM_H

#include <gts/string_list.h>
#include <rttr/variant.h>
#include <unordered_map>

namespace gts
{

GTSCORE_API int8_t   stoi8  (const std::string &str, int base = 10);
GTSCORE_API uint8_t  stoui8 (const std::string &str, int base = 10);
GTSCORE_API int16_t  stoi16 (const std::string &str, int base = 10);
GTSCORE_API uint16_t stoui16(const std::string &str, int base = 10);
GTSCORE_API int32_t  stoi32 (const std::string &str, int base = 10);
GTSCORE_API uint32_t stoui32(const std::string &str, int base = 10);
GTSCORE_API int64_t  stoi64 (const std::string &str, int base = 10);
GTSCORE_API uint64_t stoui64(const std::string &str, int base = 10);
GTSCORE_API float    stof   (const std::string &str);
GTSCORE_API double   stod   (const std::string &str);
GTSCORE_API double   stold  (const std::string &str);
GTSCORE_API bool     stob   (const std::string &str);

GTSCORE_API string_list str_split(const std::string &str, const std::string &splits, bool ignore_empty = true);
GTSCORE_API std::string str_list_join(const basic_string_list &list, const std::string &splits = " ");

GTSCORE_API bool str_starts_with(const std::string &str, const std::string &prefix);
GTSCORE_API bool str_ends_with(const std::string &str, const std::string &suffix);

GTSCORE_API std::string str_to_lower(const std::string &str);
GTSCORE_API std::string str_to_upper(const std::string &str);

GTSCORE_API std::size_t str_replace(std::string &str, const std::string &_old, const std::string &_new);
GTSCORE_API std::string str_trimmed(const std::string &str);

GTSCORE_API std::string str_remove(const std::string &str, const std::string &find);
GTSCORE_API std::string str_remove(const std::string &str, char find);

GTSCORE_API std::string from_percent_encoding(const std::string &str);

inline string_list str_split(const std::string &str, char splits, bool ignore_empty = true)
{ return str_split(str, std::string(1,splits), ignore_empty); }

GTSCORE_API std::string file_name(const std::string &file_name);
GTSCORE_API std::string file_path(const std::string &file_name);

} //namespace gts


#endif //GTS_ALGORITHM_H
