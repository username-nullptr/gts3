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
	bool global_method_call(const std::string &method_name);
	bool class_method_call(rttr::type &type, rttr::variant &obj, const std::string &method_name);
	std::pair<rttr::type, rttr::variant> &find_plugin_obj(const std::string &class_name);

private:
	service_io &m_sio;
	bool m_direct_pass_request = true;
	int m_call_method_id = 0;
};

}} //namespace gts::web


#endif //PLUGIN_SERVICE_H
