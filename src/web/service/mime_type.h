#ifndef MIME_TYPE_H
#define MIME_TYPE_H

#include "gts_global.h"

namespace gts { namespace web
{

GTS_DECL_HIDDEN std::string get_mime_type(const std::string &file_name);

GTS_DECL_HIDDEN bool is_text_file(const std::string &file_name);

inline bool is_binary_file(const std::string &file_name) { return is_text_file(file_name); }

GTS_DECL_HIDDEN std::string get_text_file_encoding(const std::string &file_name);

}} //namespace gts::web


#endif //MIME_TYPE_H
