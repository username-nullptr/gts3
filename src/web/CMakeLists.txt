set(web_sources
	main.cpp
	global.cpp
	session.cpp
	http/protocol.cpp
	http/parser.cpp
	http/request.cpp
	http/response.cpp
	service/task.cpp
	service/service.cpp
	service/service_io.cpp
	service/static_resource_service.cpp
	service/plugins_service.cpp
	service/cgi_service.cpp
)

set(web_headers
	global.h
	session.h
	gts/web_config_key.h
	gts/web_plugin_interface.h
	http/protocol.h
	http/parser.h
	http/request.h
	http/response.h
	service/task.h
	service/service.h
	service/service_io.h
	service/static_resource_service.h
	service/plugins_service.h
	service/cgi_service.h
)

add_library(gtsweb SHARED ${web_sources} ${web_headers})

target_include_directories(gtsweb PRIVATE .)
target_include_directories(gtsweb PRIVATE ../core)
target_include_directories(gtsweb PRIVATE ../log)

target_link_libraries(gtsweb PRIVATE gtscore)
target_link_libraries(gtsweb PRIVATE gtslog)

if (ENABLE_SSL)
	target_link_libraries(gtsweb PUBLIC ssl)
	target_link_libraries(gtsweb PUBLIC crypto)
endif()

set_target_properties(gtsweb PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS gtsweb DESTINATION bin)

install(FILES http/protocol.h RENAME http_protocol.h DESTINATION include/gts)

install(DIRECTORY gts
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)
