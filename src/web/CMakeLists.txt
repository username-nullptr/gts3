set(http_sources
	gts/http/global.cpp
	gts/http/request.cpp
	gts/http/response.cpp
)

set(http_headers
	gts/http/global.h
	gts/http/request.h
	gts/http/response.h
)

add_library(gtshttp SHARED ${http_sources} ${http_headers})

target_include_directories(gtshttp PRIVATE . ../core)

set_target_properties(gtshttp PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS gtshttp DESTINATION bin)

#--------------------------------------------------------------------------------------------------

set(web_sources
	main.cpp
	gts/web_global.cpp
	session/http_parser.cpp
	session/session.cpp
	service/task.cpp
	service/service.cpp
	service/service_io.cpp
	service/static_resource_service.cpp
	service/plugins_service.cpp
	service/cgi_service.cpp
)

set(web_headers
	gts/web_global.h
	gts/web_config_key.h
	gts/web_plugin_interface.h
	session/http_parser.h
	session/session.h
	service/task.h
	service/service.h
	service/service_io.h
	service/static_resource_service.h
	service/plugins_service.h
	service/cgi_service.h
)

add_library(gtsweb SHARED ${web_sources} ${web_headers})

target_include_directories(gtsweb PRIVATE . ../core ../log)
target_link_libraries(gtsweb PRIVATE gtscore gtslog gtshttp)

set_target_properties(gtsweb PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS gtsweb DESTINATION bin)

install(DIRECTORY gts
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)
