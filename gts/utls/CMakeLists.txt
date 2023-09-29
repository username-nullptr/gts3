set(utls_sources
	json.cpp
)

set(utls_headers
	registration.h
	json.h
)

if (ENABLE_DBI)
	list(APPEND utls_headers
		db_convert.h
		db_convert_detail.h
	)
endif()

set(target_name gtsutls)

add_library(${target_name} SHARED ${utls_sources} ${utls_headers})
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
