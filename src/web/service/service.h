#include "service/plugins_service.h"
#include "service/cgi_service.h"

namespace gts { namespace web
{

class GTS_DECL_HIDDEN service
{
public:
	static void call_plugin_service(service_io &sio);
	static void call_cgi_service(service_io &sio);

public:
	static void init();
	static void exit();
};

}} //namespace gts::web
