#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include "gts/web/registration.h"

namespace gts { namespace web
{

class service_io;

class GTS_DECL_HIDDEN plugin_service
{
	GTS_DISABLE_COPY_MOVE(plugin_service)

public:
	plugin_service(service_io &sio);
	void call();

public:
	static void init();
	static void exit();
	static std::string view_status();
	static void new_websocket(service_io &sio);

private:
	rttr::variant global_method_call(const rttr::method &method, const rttr::type &p1_type);
	rttr::variant class_method_call(rttr::method &method, rttr::variant &obj, const rttr::type &p1_type);
	registration::service *find_filter(const std::string &url);

private:
	service_io &m_sio;
};

}} //namespace gts::web


#endif //PLUGIN_SERVICE_H
