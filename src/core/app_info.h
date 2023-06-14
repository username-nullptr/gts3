#ifndef APP_INFO_H
#define APP_INFO_H

#include "gts/gts_global.h"

namespace gts { namespace appinfo
{

GTSCORE_API std::string set_instance_name(const std::string &name);
GTSCORE_API std::string instance_name();

GTSCORE_API std::string file_path();
GTSCORE_API std::string dir_path();

GTSCORE_API std::string tmp_dir_path();
GTSCORE_API std::string lock_file_name();

GTSCORE_API std::string current_directory();
GTSCORE_API bool set_current_directory(const std::string &path);

GTSCORE_API std::string absolute_path(const std::string &path);
GTSCORE_API bool is_absolute_path(const std::string &path);

}} //namespace gts::appinfo


#endif //APP_INFO_H
