set(log_sources
	gts/log.cpp
	gts/log_buffer.cpp
)

set(log_headers
	gts/log_global.h
	gts/log_buffer.h
	gts/log.h
)

add_library(gtslog SHARED ${log_sources} ${log_headers})
target_compile_definitions(gtslog PRIVATE gtslog_EXPORTS)

target_include_directories(gtslog PRIVATE . ../core)

set_target_properties(gtslog PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS gtslog DESTINATION bin)
install(FILES ${log_headers} DESTINATION include/gts)
