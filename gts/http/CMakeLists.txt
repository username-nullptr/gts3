set(http_sources
	types.cpp
	cookies.cpp
	session.cpp
	request.cpp
	response.cpp
	parser.cpp
	service_context.cpp
)

set(http_gts_headers
	http_global.h
	value.h
	container.h
	types.h
	cookies.h
	session.h
	request.h
	response.h
	formatter.h
	parser.h
	service_context.h
)

set(http_sps
	request_impl.cpp
	request_impl.h
)

set(target_name gtshttp)

add_library(${target_name} SHARED ${http_sources} ${http_gts_headers} ${http_sps})
target_compile_definitions(${target_name} PRIVATE gtshttp_EXPORTS)
target_link_libraries(${target_name} PUBLIC gtscore)

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)
install(FILES ${http_gts_headers} DESTINATION include/gts/http)
