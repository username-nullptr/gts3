#ifndef GTS_HTTP_TYPES_H
#define GTS_HTTP_TYPES_H

#include <gts/http/cookies.h>
#include <gts/http/session.h>

namespace gts { namespace http
{

enum status
{
	hs_continue                        = 100, // Continue
	hs_switching_protocols             = 101, // Switching Protocols
	hs_processing                      = 102, // Processing
	hs_ok                              = 200, // OK
	hs_created                         = 201, // Created
	hs_accepted                        = 202, // Accepted
	hs_non_authoritative_information   = 203, // Non-Authoritative Information
	hs_no_content                      = 204, // No Content
	hs_reset_content                   = 205, // Reset Content
	hs_partial_content                 = 206, // Partial Content
	hs_multi_status                    = 207, // Multi-Status
	hs_already_reported                = 208, // Already Reported
	hs_im_used                         = 226, // IM Used
	hs_multiple_choices                = 300, // Multiple Choices
	hs_moved_permanently               = 301, // Moved Permanently
	hs_found                           = 302, // Found
	hs_see_other                       = 303, // See Other
	hs_not_modified                    = 304, // Not Modified
	hs_use_proxy                       = 305, // Use Proxy
	hs_temporary_redirect              = 307, // Temporary Redirect
	hs_permanent_redirect              = 308, // Permanent Redirect
	hs_bad_request                     = 400, // Bad Request
	hs_unauthorized                    = 401, // Unauthorized
	hs_payment_required                = 402, // Payment Required
	hs_forbidden                       = 403, // Forbidden
	hs_not_found                       = 404, // Not Found
	hs_method_not_allowed              = 405, // Method Not Allowed
	hs_not_acceptable                  = 406, // Not Acceptable
	hs_proxy_authentication_required   = 407, // Proxy Authentication Required
	hs_request_timeout                 = 408, // Request Timeout
	hs_conflict                        = 409, // Conflict
	hs_gone                            = 410, // Gone
	hs_length_required                 = 411, // Length Required
	hs_precondition_failed             = 412, // Precondition Failed
	hs_payload_too_large               = 413, // Payload Too Large
	hs_uri_too_long                    = 414, // URI Too Long
	hs_unsupported_media_type          = 415, // Unsupported Media Type
	hs_range_not_satisfiable           = 416, // Range Not Satisfiable
	hs_expectation_failed              = 417, // Expectation Failed
	hs_misdirected_request             = 421, // Misdirected Request
	hs_unprocessable_entity            = 422, // Unprocessable Entity
	hs_locked                          = 423, // Locked
	hs_failed_dependency               = 424, // Failed Dependency
	hs_upgrade_required                = 426, // Upgrade Required
	hs_precondition_required           = 428, // Precondition Required
	hs_tooMany_requests                = 429, // Too Many Requests
	hs_request_header_fields_too_large = 431, // Request Header Fields Too Large
	hs_unavailable_for_legal_reasons   = 451, // Unavailable For Legal Reasons
	hs_internal_server_error           = 500, // Internal Server Error
	hs_not_implemented                 = 501, // Not Implemented
	hs_bad_gateway                     = 502, // Bad Gateway
	hs_service_unavailable             = 503, // Service Unavailable
	hs_gateway_timeout                 = 504, // Gateway Timeout
	hs_http_version_not_supported      = 505, // HTTP Version Not Supported
	hs_variant_also_negotiates         = 506, // Variant Also Negotiates
	hs_insufficient_storage            = 507, // Insufficient Storage
	hs_loop_detected                   = 508, // Loop Detected
	hs_not_extended                    = 510, // Not Extended
	hs_network_authentication_required = 511, // Network Authentication Required
};

GTSWEB_API std::string status_description(int s);

enum method
{
	METHOD_UNKNOWN = -1,
	GET,
	PUT,
	POST,
	HEAD,
	DELETE,
	OPTIONS,
	CONNECT,
	TRACH
};
GTSWEB_API std::string method_string(method m);
GTSWEB_API method from_method_string(const std::string &m);

enum class redirect_type
{
	moved_permanently,  //301
	permanent_redirect, //308
	found,              //302
	see_other,          //303
	temporary_redirect, //307
	multiple_choices,   //300
	not_modified        //304
};

using header  = http::pair<value>;
using headers = http::map<http::less_case_insensitive, value>;

using parameter  = http::pair<value>;
using parameters = http::unordered_map<value>;

}} //namespace gts::http


#endif //GTS_HTTP_TYPES_H
