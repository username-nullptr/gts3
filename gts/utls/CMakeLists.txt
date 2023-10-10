set(target_name gtsutls)

set(utls_sources
	json.cpp
)

set(utls_headers
	global.h
	registration.h
	json.h
)

set(utls_detail_headers
	detail/json.h
)

if (ENABLE_DBI)
	list(APPEND utls_headers
		db_convert.h
	)
	list(APPEND utls_detail_headers
		detail/db_convert.h
	)
endif()

add_library(${target_name} SHARED ${utls_sources} ${utls_headers} ${utls_detail_headers})
target_compile_definitions(${target_name} PRIVATE ${target_name}_EXPORTS)

target_include_directories(${target_name} PRIVATE .)

if (ENABLE_DBI)
	target_link_libraries(${target_name} PUBLIC gtsdbi)
else()
	target_link_libraries(${target_name} PUBLIC rttr_core)
endif()

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)
install(FILES ${utls_headers} DESTINATION include/gts/utls)
install(FILES ${utls_detail_headers} DESTINATION include/gts/utls/detail)
