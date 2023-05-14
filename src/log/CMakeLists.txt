set(log_sources
	log.cpp
	log_buffer.cpp
)

set(log_headers
	log_global.h
	log_operator.h
	log_buffer.h
	log.h
)

add_library(gtslog SHARED ${log_sources} ${log_headers})

target_include_directories(gtslog PRIVATE .)
target_include_directories(gtslog PRIVATE ../core)

set_target_properties(gtslog PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
