#include <string>
#include <rttr/type>

namespace io
{
/*!
 * Deserialize the given json string \p json to the given instance \p obj.
 */
bool from_json(const std::string& json, rttr::instance obj);

}

