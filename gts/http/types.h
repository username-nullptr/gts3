
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef GTS_HTTP_TYPES_H
#define GTS_HTTP_TYPES_H

#include <gts/http/cookies.h>
#include <gts/http/session.h>
#include <gts/flags.h>

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
GTS_HTTP_API std::string status_description(int s);

enum method
{
	METHOD_UNKNOWN = -1,
	GET     = 0x01,
	PUT     = 0x02,
	POST    = 0x04,
	HEAD    = 0x08,
	DELETE  = 0x10,
	OPTIONS = 0x20,
	CONNECT = 0x40,
	TRACH   = 0x80
};
GTS_DECLARE_FLAGS(method_flags, method)

GTS_HTTP_API std::string method_string(method m);
GTS_HTTP_API method from_method_string(const std::string &m);

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

struct header : http::pair<value>
{
	using http::pair<value>::pair;
#if GTS_CPLUSPLUS >= 201703L
	static constexpr const char *accept_language   = "Accept-Language";
	static constexpr const char *accept_encoding   = "Accept-Encoding";
	static constexpr const char *accept_ranges     = "Accept-Ranges";
	static constexpr const char *accept            = "Accept";
	static constexpr const char *age               = "Age";
	static constexpr const char *content_encoding  = "Content-Encoding";
	static constexpr const char *content_length    = "Content-Length";
	static constexpr const char *cache_control     = "Cache-Control";
	static constexpr const char *content_range     = "Content-Range";
	static constexpr const char *content_type      = "Content-Type";
	static constexpr const char *connection        = "Connection";
	static constexpr const char *expires           = "Expires";
	static constexpr const char *host              = "Host";
	static constexpr const char *last_modified     = "Last-Modified";
	static constexpr const char *location          = "Location";
	static constexpr const char *referer           = "Referer";
	static constexpr const char *range             = "Range";
	static constexpr const char *transfer_encoding = "Transfer-Encoding";
	static constexpr const char *user_agent        = "User-Agent";
#else //c++17
	static const char *accept_language;
	static const char *accept_encoding;
	static const char *accept_ranges;
	static const char *accept;
	static const char *age;
	static const char *content_encoding;
	static const char *content_length;
	static const char *cache_control;
	static const char *content_range;
	static const char *content_type;
	static const char *connection;
	static const char *expires;
	static const char *host;
	static const char *last_modified;
	static const char *location;
	static const char *referer;
	static const char *range;
	static const char *transfer_encoding;
	static const char *user_agent;
#endif //c++17
};
using headers = http::map<value, http::less_case_insensitive>;

using parameter  = http::pair<value>;
using parameters = http::unordered_map<value>;

}} //namespace gts::http


#endif //GTS_HTTP_TYPES_H
