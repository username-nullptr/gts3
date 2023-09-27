set(dbi_sources
	result_iterator.cpp
	driver.cpp
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
	detail/cell.h
	detail/connection.h
)

set(target_name gtsdbi)

add_library(${target_name} SHARED ${dbi_sources} ${dbi_headers})
target_compile_definitions(${target_name} PRIVATE gtsdbi_EXPORTS)

target_include_directories(${target_name} PRIVATE .)
target_link_libraries(${target_name} PUBLIC gtscore)

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)
install(FILES ${dbi_headers} DESTINATION include/gts/dbi)
