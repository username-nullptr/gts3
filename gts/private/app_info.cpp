#include "app_info.h"

namespace gts { namespace app
{

#define APP_TMP_DIR_PATH  "/tmp/gts3/"

std::string tmp_dir_path()
{
	return APP_TMP_DIR_PATH + instance_name() + "/";
}

std::string lock_file_name()
{
	return tmp_dir_path() + "pid.lock";
}

}} //namespace gts::app
