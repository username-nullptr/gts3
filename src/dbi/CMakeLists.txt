set(dbi_sources
	cell.cpp
	result_set.cpp
	driver.cpp
	execute.cpp
	manager.cpp
)

set(dbi_headers
	dbi_global.h
	error.h
	utility.h
	types.h
	cell.h
	format_ex.h
	result_set.h
	driver.h
	execute.h
	manager.h
)

set(target_name gtsdbi)

add_library(${target_name} SHARED ${dbi_sources} ${dbi_headers})
target_compile_definitions(${target_name} PRIVATE gtsdbi_EXPORTS)

target_include_directories(${target_name} PRIVATE . .. ../core)
target_link_libraries(${target_name} PUBLIC gtscore)

set_target_properties(gtsdbi PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../${output_dir_name}/bin
)
install(TARGETS gtsdbi DESTINATION bin)
