#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include "gts/web/registration.h"

namespace gts { namespace web
{

class service_io;

class GTS_DECL_HIDDEN plugins_service
{
	GTS_DISABLE_COPY_MOVE(plugins_service)

public:
	plugins_service(service_io &sio);
	bool exists();

public:
	bool call();
	bool call_filter();

private:
	registration::service_array *check();
	rttr::variant global_method_call(const rttr::method &method, const rttr::type &p1_type);
	rttr::variant class_method_call(rttr::method &method, rttr::variant &obj, const rttr::type &p1_type);
	registration::service *find_filter(const std::string &url);

private:
	service_io &m_sio;
};

}} //namespace gts::web


#endif //PLUGIN_SERVICE_H
