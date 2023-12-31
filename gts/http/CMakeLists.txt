set(target_name gtshttp)

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
	global.h
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

set(http_gts_detail_headers
	detail/container.h
	detail/cookies.h
	detail/session.h
	detail/request.h
	detail/response.h
)

set(http_sps
	detail/request_impl.cpp
	detail/request_impl.h
)

set(all_files
	${http_sources}
	${http_gts_headers}
	${http_gts_detail_headers}
	${http_sps}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_files})
add_library(${target_name} SHARED ${all_files})

target_compile_definitions(${target_name} PRIVATE ${target_name}_EXPORTS)
target_link_libraries(${target_name} PUBLIC gtscore)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
	message("------------------------------------------------------------")
	message("  *** Warning: Unknown bugs may occur in the GNU(32bit).")
	message("------------------------------------------------------------")
	target_compile_options(${target_name} PRIVATE -fno-stack-protector)
	target_link_options(${target_name} PRIVATE -fno-stack-protector)
endif()

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)
install(FILES ${http_gts_headers} DESTINATION include/gts/http)
install(FILES ${http_gts_detail_headers} DESTINATION include/gts/http/detail)
