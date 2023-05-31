set(log_sources
	gts/log.cpp
	gts/log_buffer.cpp
	private/rw_lock.cpp
)

set(log_headers
	gts/log_global.h
	gts/log_buffer.h
	gts/log_operator.h
	gts/log.h
)

set(log_private_headers
	private/rw_lock.h
)

add_library(gtslog SHARED ${log_sources} ${log_headers} ${log_private_headers})

target_include_directories(gtslog PRIVATE . ../core)

set_target_properties(gtslog PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS gtslog DESTINATION bin)
install(FILES ${log_headers} DESTINATION include/gts)
