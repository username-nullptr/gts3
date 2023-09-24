set(web_sources
	main.cpp
	gts/http/types.cpp
	gts/http/cookies.cpp
	gts/http/session.cpp
	gts/http/request.cpp
	gts/http/response.cpp
	gts/web/registration.cpp
	gts/web/socket.cpp
	session/request_impl.cpp
	session/http_parser.cpp
	session/session.cpp
	service/task.cpp
	service/service_io.cpp
	service/cgi_service.cpp
	service/plugins_service.cpp
)

set(web_headers
	gts/web.h
	gts/http.h
	gts/web_global.h
	gts/http/value.h
	gts/http/container.h
	gts/http/types.h
	gts/http/cookies.h
	gts/http/session.h
	gts/http/request.h
	gts/http/response.h
	gts/http/formatter.h
	gts/web/types.h
	gts/web/config_key.h
	gts/web/thread_pool.h
	gts/web/registration.h
	gts/web/socket.h
	session/request_impl.h
	session/http_parser.h
	session/session.h
	service/task.h
	service/service_io.h
	service/cgi_service.h
	service/plugins_service.h
)

set(target_name gtsweb)

add_library(${target_name} SHARED ${web_sources} ${web_headers})
target_compile_definitions(${target_name} PRIVATE gtsweb_EXPORTS)

target_include_directories(${target_name} PRIVATE . ../core)
target_link_libraries(${target_name} PUBLIC gtscore)

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS ${target_name} DESTINATION bin)

install(DIRECTORY gts
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)
