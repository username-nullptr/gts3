set(core_sources
	gts_global.cpp
	gts_algorithm.cpp
	process.cpp
	settings.cpp
)

set(core_headers
	gts_global.h
	gts_algorithm.h
	gts_config_key.h
	process.h
	settings.h
	private/gts_global_p.h
	private/process_p.h
)

if (WIN32)
	list(APPEND core_sources
		private/gts_global_win.cpp
		private/process_win.cpp
	)
elseif (UNIX)
	list(APPEND core_sources
		private/gts_global_unix.cpp
		private/process_unix.cpp
	)
endif()

add_library(gtscore SHARED ${core_sources} ${core_headers})

target_include_directories(gtscore PRIVATE .)

set_target_properties(gtscore PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
