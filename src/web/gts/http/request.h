#ifndef GTS_HTTP_REQUEST_H
#define GTS_HTTP_REQUEST_H

#include <gts/http/global.h>

namespace gts { namespace http
{

struct GTSHTTP_API request
{
	std::string method;
	std::string version;
	std::string path;

	http::parameters parameters;
	http::headers headers;

	std::string parameters_string;
	mutable std::string body;

	bool keep_alive = true;
	bool support_gzip = false;

	inline request() {}
	request(request &&other);
	request &operator=(request &&other);

	bool is_valid() const;
	void finish();
};

}} //namespace gts::http


#endif //GTS_HTTP_REQUEST_H
