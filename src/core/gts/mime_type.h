#ifndef GTS_MIME_TYPE_H
#define GTS_MIME_TYPE_H

#include <gts/gts_global.h>

namespace gts
{

GTSCORE_API std::string get_mime_type(const std::string &file_name);

GTSCORE_API bool is_text_file(const std::string &file_name);

inline bool is_binary_file(const std::string &file_name) { return is_text_file(file_name); }

GTSCORE_API std::string get_text_file_encoding(const std::string &file_name);

} //namespace gts


#endif //GTS_MIME_TYPE_H
