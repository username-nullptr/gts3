#ifndef PLUGIN_SERVICE_H
#define PLUGIN_SERVICE_H

#include "gts/gts_global.h"

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

private:
	void global_method_call(const rttr::method &method);
	void class_method_call(rttr::method &method, rttr::variant &obj);

private:
	service_io &m_sio;
	bool m_direct_pass_request = true;
	int m_call_method_id = 0;
};

}} //namespace gts::web


#endif //PLUGIN_SERVICE_H
