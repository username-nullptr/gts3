#ifndef APP_INFO_H
#define APP_INFO_H

#include "gts/application.h"

namespace gts { namespace app
{

GTSCORE_API std::string set_instance_name(const std::string &name);

GTSCORE_API std::string tmp_dir_path();

GTSCORE_API std::string lock_file_name();


}} //namespace gts::app


#endif //APP_INFO_H
