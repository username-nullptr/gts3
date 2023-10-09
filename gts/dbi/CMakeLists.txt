set(target_name gtsdbi)

set(dbi_sources
	result_iterator.cpp
	connection.cpp
	manager.cpp
)

set(dbi_headers
	global.h
	error.h
	types.h
	cell.h
	formatter.h
	result_iterator.h
	driver.h
	connection.h
	manager.h
)

set(dbi_detail_headers
	detail/error.h
	detail/cell.h
	detail/driver.h
	detail/connection.h
)

add_library(${target_name} SHARED ${dbi_sources} ${dbi_headers} ${dbi_detail_headers})
target_compile_definitions(${target_name} PRIVATE gtsdbi_EXPORTS)

target_include_directories(${target_name} PRIVATE .)
target_link_libraries(${target_name} PUBLIC gtscore)

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)
install(FILES ${dbi_headers} DESTINATION include/gts/dbi)
install(FILES ${dbi_detail_headers} DESTINATION include/gts/dbi/detail)
