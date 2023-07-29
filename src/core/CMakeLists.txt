set(core_sources
	gts/gts_global.cpp
	gts/args_parser.cpp
	gts/application.cpp
	gts/string_list.cpp
	gts/algorithm.cpp
	gts/mime_type.cpp
	gts/settings.cpp
	gts/tcp_socket.cpp
	gts/ssl_socket.cpp
	gts/registration.cpp
	gts/rw_mutex.cpp
	gts/uuid.cpp
	app_info.cpp
	process.cpp
	server_tool.cpp
)

set(core_headers
	gts/cplusplus.hpp
	gts/utility.h
	gts/gts_global.h
	gts/args_parser.h
	gts/application.h
	gts/string_list.h
	gts/algorithm.h
	gts/mime_type.h
	gts/gts_config_key.h
	gts/settings.h
	gts/tcp_socket.h
	gts/ssl_socket.h
	gts/registration.h
	gts/formatter.h
	gts/exception.h
	gts/rw_mutex.h
	gts/uuid.h
	app_info.h
	process.h
	server_tool.h
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

add_library(gtscore SHARED ${core_sources} ${core_headers})
target_compile_definitions(gtscore PRIVATE gtscore_EXPORTS)

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
