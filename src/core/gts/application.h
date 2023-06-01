#ifndef GTS_APPLICATION_H
#define GTS_APPLICATION_H

#include <gts/gts_global.h>

namespace gts { namespace app
{

GTSCORE_API std::string file_path();
GTSCORE_API std::string dir_path();

GTSCORE_API std::string current_directory();

GTSCORE_API std::string absolute_path(const std::string &path);
GTSCORE_API bool is_absolute_path(const std::string &path);

}} //namespace gts::app


#endif //GTS_APPLICATION_H
