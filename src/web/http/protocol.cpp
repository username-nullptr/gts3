#include "protocol.h"
#include <assert.h>

namespace gts::web::http
{

std::string status_description(int s)
{
	static std::unordered_map<status, std::string> hash
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
	auto it = hash.find(static_cast<status>(s));
	assert(it != hash.end());
	return it->second;
}

} //namespace gts::web::http
