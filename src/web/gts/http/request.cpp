#include "request.h"
#include "gts/algorithm.h"

namespace gts { namespace http
{

request::request(request &&other)
{
	operator=(std::move(other));
}

request &request::operator=(request &&other)
{
	method  = std::move(other.method);
	version = std::move(other.version);
	path    = std::move(other.path);

	parameters = std::move(other.parameters);
	headers    = std::move(other.headers);

	parameters_string = std::move(other.parameters_string);
	body = std::move(other.body);

	keep_alive   = other.keep_alive;
	support_gzip = other.support_gzip;
	return *this;
}

bool request::is_valid() const
{
	return not version.empty() and not path.empty();
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
