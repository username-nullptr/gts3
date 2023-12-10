#ifndef GTS_WEB_SOCKET_PROTOCOL_H
#define GTS_WEB_SOCKET_PROTOCOL_H

#include <gts/web/global.h>

GTS_WEB_SOCKET_PROTOCOL_NAMESPACE_BEGIN

enum version
{
	version_unknown = -1,
	version0  = 0,
	version1  = 1,
	version2  = 2,
	version3  = 3,
	version4  = 4,
	version5  = 5,
	version6  = 6,
	version7  = 7,
	version8  = 8,
	version13 = 13,
	version_latest = version13
};

enum close_code
{
	cc_normal                 = 1000,
	cc_going_away             = 1001,
	cc_protocol_error         = 1002,
	cc_datatype_not_supported = 1003,
	cc_reserved_1004          = 1004,
	cc_missing_status_code    = 1005,
	cc_abnormal_disconnection = 1006,
	cc_wrong_datatype         = 1007,
	cc_policy_violated        = 1008,
	cc_too_much_data          = 1009,
	cc_missing_extension      = 1010,
	cc_bad_operation          = 1011,
	cc_tls_handshake_failed   = 1015
};

enum op_code
{
	oc_continue   = 0x00,
	oc_text       = 0x01,
	oc_binary     = 0x02,
	oc_reserved3  = 0x03,
	oc_reserved4  = 0x04,
	oc_reserved5  = 0x05,
	oc_reserved6  = 0x06,
	oc_reserved7  = 0x07,
	oc_close      = 0x08,
	oc_ping       = 0x09,
	oc_pong       = 0x0A,
	oc_reserved_b = 0x0B,
	oc_reserved_c = 0x0C,
	oc_reserved_d = 0x0D,
	oc_reserved_e = 0x0E,
	oc_reserved_f = 0x0F
};

GTS_WEB_API bool is_op_code_reserved(op_code code);

GTS_WEB_SOCKET_PROTOCOL_NAMESPACE_END


#endif //GTS_WEB_SOCKET_PROTOCOL_H
