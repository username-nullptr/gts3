#ifndef GTS_WEB_TYPES_H
#define GTS_WEB_TYPES_H

#include <gts/http/container.h>

namespace gts { namespace web
{

using environment = http::pair<http::value>;

using environments = http::unordered_map<http::value>;

enum class data_type {
	text, binary
};

struct buffer
{
	data_type type;
	std::string data;
};

}} //namespace gts::web


#endif //GTS_WEB_TYPES_H
