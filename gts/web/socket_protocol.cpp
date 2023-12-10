#include "socket_protocol.h"

GTS_WEB_SOCKET_PROTOCOL_NAMESPACE_BEGIN

bool is_op_code_reserved(op_code code)
{
	return ((code > oc_binary) and (code < oc_close)) or (code > oc_pong);
}

GTS_WEB_SOCKET_PROTOCOL_NAMESPACE_END
