
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

#include "types.h"
#include <assert.h>

namespace gts { namespace http
{

std::string status_description(int s)
{
	static std::unordered_map<int, std::string> hash
	{
		{ hs_continue                        , "Continue"                        },
		{ hs_switching_protocols             , "Switching Protocols"             },
		{ hs_processing                      , "Processing"                      },
		{ hs_ok                              , "OK"                              },
		{ hs_created                         , "Created"                         },
		{ hs_accepted                        , "Accepted"                        },
		{ hs_non_authoritative_information   , "Non-Authoritative Information"   },
		{ hs_no_content                      , "No Content"                      },
		{ hs_reset_content                   , "Reset Content"                   },
		{ hs_partial_content                 , "Partial Content"                 },
		{ hs_multi_status                    , "Multi-Status"                    },
		{ hs_already_reported                , "Already Reported"                },
		{ hs_im_used                         , "IM Used"                         },
		{ hs_multiple_choices                , "Multiple Choices"                },
		{ hs_moved_permanently               , "Moved Permanently"               },
		{ hs_found                           , "Found"                           },
		{ hs_see_other                       , "See Other"                       },
		{ hs_not_modified                    , "Not Modified"                    },
		{ hs_use_proxy                       , "Use Proxy"                       },
		{ hs_temporary_redirect              , "Temporary Redirect"              },
		{ hs_permanent_redirect              , "Permanent Redirect"              },
		{ hs_bad_request                     , "Bad Request"                     },
		{ hs_unauthorized                    , "Unauthorized"                    },
		{ hs_payment_required                , "Payment Required"                },
		{ hs_forbidden                       , "Forbidden"                       },
		{ hs_not_found                       , "Not Found"                       },
		{ hs_method_not_allowed              , "Method Not Allowed"              },
		{ hs_not_acceptable                  , "Not Acceptable"                  },
		{ hs_proxy_authentication_required   , "Proxy Authentication Required"   },
		{ hs_request_timeout                 , "Request Timeout"                 },
		{ hs_conflict                        , "Conflict"                        },
		{ hs_gone                            , "Gone"                            },
		{ hs_length_required                 , "Length Required"                 },
		{ hs_precondition_failed             , "Precondition Failed"             },
		{ hs_payload_too_large               , "Payload Too Large"               },
		{ hs_uri_too_long                    , "URI Too Long"                    },
		{ hs_unsupported_media_type          , "Unsupported Media Type"          },
		{ hs_range_not_satisfiable           , "Range Not Satisfiable"           },
		{ hs_expectation_failed              , "Expectation Failed"              },
		{ hs_misdirected_request             , "Misdirected Request"             },
		{ hs_unprocessable_entity            , "Unprocessable Entity"            },
		{ hs_locked                          , "Locked"                          },
		{ hs_failed_dependency               , "Failed Dependency"               },
		{ hs_upgrade_required                , "Upgrade Required"                },
		{ hs_precondition_required           , "Precondition Required"           },
		{ hs_tooMany_requests                , "Too Many Requests"               },
		{ hs_request_header_fields_too_large , "Request Header Fields Too Large" },
		{ hs_unavailable_for_legal_reasons   , "Unavailable For Legal Reasons"   },
		{ hs_internal_server_error           , "Internal Server Error"           },
		{ hs_not_implemented                 , "Not Implemented"                 },
		{ hs_bad_gateway                     , "Bad Gateway"                     },
		{ hs_service_unavailable             , "Service Unavailable"             },
		{ hs_gateway_timeout                 , "Gateway Timeout"                 },
		{ hs_http_version_not_supported      , "HTTP Version Not Supported"      },
		{ hs_variant_also_negotiates         , "Variant Also Negotiates"         },
		{ hs_insufficient_storage            , "Insufficient Storage"            },
		{ hs_loop_detected                   , "Loop Detected"                   },
		{ hs_not_extended                    , "Not Extended"                    },
		{ hs_network_authentication_required , "Network Authentication Required" },
	};
	auto it = hash.find(s);
	assert(it != hash.end());
	return it->second;
}

std::string method_string(method m)
{
	switch(m)
	{
	case GET    : return "GET";
	case PUT    : return "PUT";
	case POST   : return "POST";
	case HEAD   : return "HEAD";
	case DELETE : return "DELETE";
	case OPTIONS: return "OPTIONS";
	case CONNECT: return "CONNECT";
	case TRACH  : return "TRACH";
	default: break;
	}
	assert(false);
	return "Unkown";
}

gts::http::method from_method_string(const std::string &m)
{
	if     ( m == "GET"     ) return GET;
	else if( m == "PUT"     ) return PUT;
	else if( m == "POST"    ) return POST;
	else if( m == "HEAD"    ) return HEAD;
	else if( m == "DELETE"  ) return DELETE;
	else if( m == "OPTIONS" ) return OPTIONS;
	else if( m == "CONNECT" ) return CONNECT;
	else if( m == "TRACH"   ) return TRACH;
	return METHOD_UNKNOWN;
}

#if GTS_CPLUSPLUS < 201703L
const char *header::accept_language   = "Accept-Language";
const char *header::accept_encoding   = "Accept-Encoding";
const char *header::accept_ranges     = "Accept-Ranges";
const char *header::accept            = "Accept";
const char *header::age               = "Age";
const char *header::content_encoding  = "Content-Encoding";
const char *header::content_length    = "Content-Length";
const char *header::cache_control     = "Cache-Control";
const char *header::content_range     = "Content-Range";
const char *header::content_type      = "Content-Type";
const char *header::connection        = "Connection";
const char *header::expires           = "Expires";
const char *header::host              = "Host";
const char *header::last_modified     = "Last-Modified";
const char *header::location          = "Location";
const char *header::origin            = "Origin";
const char *header::referer           = "Referer";
const char *header::range             = "Range";
const char *header::transfer_encoding = "Transfer-Encoding";
const char *header::user_agent        = "User-Agent";
const char *header::upgrade           = "Upgrade";
#endif //c++17

}} //namespace gts::http
