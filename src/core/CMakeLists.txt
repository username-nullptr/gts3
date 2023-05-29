set(core_sources
	gts/gts_global.cpp
	gts/algorithm.cpp
	gts/mime_type.cpp
	json.cpp
	app_info.cpp
	socket.cpp
	process.cpp
	settings.cpp
)

set(core_headers
	gts/gts_global.h
	gts/algorithm.h
	gts/mime_type.h
	gts/gts_config_key.h
	json.h
	app_info.h
	socket.h
	process.h
	settings.h
	private/process_p.h
)

if (WIN32)
	list(APPEND core_sources
		private/app_info_win.cpp
		private/process_win.cpp
	)
elseif (UNIX)
	list(APPEND core_sources
		private/app_info_unix.cpp
		private/process_unix.cpp
	)
endif()

if (ENABLE_SSL)
	list(APPEND core_sources gts/ssl.cpp)
	list(APPEND core_headers gts/ssl.h)
endif()

add_library(gtscore SHARED ${core_sources} ${core_headers})

target_include_directories(gtscore PRIVATE .)

if (ENABLE_SSL)
	target_link_libraries(gtscore PUBLIC ssl crypto)
endif()

target_link_libraries(gtscore PUBLIC rttr_core)

set_target_properties(gtscore PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS gtscore DESTINATION bin)

install(DIRECTORY gts
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)
