#ifndef SERVICE_H
#define SERVICE_H

#include "http/response.h"
#include "http/request.h"

namespace gts { namespace web
{

class GTS_DECL_HIDDEN service
{
public:
	service(tcp::socket &socket, http::request &request);

public:
	static void init();
	static void exit();

public:
	void call_static_resource(const std::string &url_name);
	void call_plugins(const std::string &url_name);
	void call_cgi(const std::string &url_name);

public:
	void return_to_null(http::status status = http::hs_ok);

public:
	tcp::socket &socket;
	http::request &request;
	http::response response;
};

}} //namespace gts::web


#endif //SERVICE_H
