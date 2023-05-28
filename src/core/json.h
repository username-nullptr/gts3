#include "gts/gts_global.h"
#include <rapidjson/document.h>

namespace rapidjson
{

GTSCORE_API std::string from_file(const std::string &file, rapidjson::Document &json);

} //namespace rapidjson
