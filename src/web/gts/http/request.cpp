#include "request.h"
#include "gts/algorithm.h"

namespace gts { namespace http
{

bool request::is_valid() const
{
	return not method.empty();
}

void request::finish()
{
	auto it = headers.find("connection");
	if( it == headers.end() )
		keep_alive = version != "1.0";
	else
		keep_alive = to_lower(it->second) != "close";

	it = headers.find("accept-encoding");
	if( it == headers.end() )
		support_gzip = false;
	else
	{
		for(auto &str : string_split(it->second, ","))
		{
			if( to_lower(trimmed(str)) == "gzip" )
			{
				support_gzip = true;
				break;
			}
		}
	}
}

}} //namespace gts::http
