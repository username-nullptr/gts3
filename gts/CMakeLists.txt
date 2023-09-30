project(GTS VERSION 3.0.0)
add_definitions(-DGTS_VERSION_STR="3.0.0 dev5")

if (WIN32)
	set(OS_CPP win)
elseif (UNIX)
	set(OS_CPP unix)
endif()

set(core_sources
	global.cpp
	args_parser.cpp
	application.cpp
	string_list.cpp
	algorithm.cpp
	mime_type.cpp
	settings.cpp
	tcp_socket.cpp
	ssl_socket.cpp
	registration.cpp
	uuid.cpp
	log_buffer.cpp
	log.cpp
	private/app_info.cpp
	private/application_${OS_CPP}.cpp
	private/tcp_socket_${OS_CPP}.cpp
)

set(core_gts_headers
	cplusplus.hpp
	utility.h
	global.h
	args_parser.h
	application.h
	string_list.h
	algorithm.h
	mime_type.h
	gts_config_key.h
	settings.h
	tcp_socket.h
	ssl_socket.h
	registration.h
	formatter.h
	exception.h
	rw_mutex.h
	flags.h
	uuid.h
	chrono.h
	log_buffer.h
	log.h
)

set(core_gts_detail_headers
	detail/application.h
	detail/settings.h
	detail/tcp_socket.h
	detail/registration.h
	detail/exception.h
	detail/rw_mutex.h
	detail/flags.h
	detail/log_buffer.h
	detail/log.h
)

set(core_sp_headers
	gts_enable_ssl.h
	private/app_info.h
)

if (ENABLE_SSL)
	list(APPEND core_sp_headers gts_enable_ssl_p.h)
else()
	list(APPEND core_sp_headers gts_disable_ssl_p.h)
endif()

set(target_name gtscore)

add_library(${target_name} SHARED
	${core_sources}
	${core_gts_headers}
	${core_gts_detail_headers}
	${core_sp_headers}
)
target_compile_definitions(${target_name} PRIVATE gtscore_EXPORTS)

if (ENABLE_SSL)
	target_link_libraries(${target_name} PUBLIC ssl crypto)
endif()

target_link_libraries(${target_name} PUBLIC rttr_core)
if(UNIX)
	target_link_libraries(${target_name} PUBLIC pthread)
endif()

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../${output_dir_name}/bin
)

install(TARGETS ${target_name} DESTINATION bin)
install(FILES ${core_gts_headers} DESTINATION include/gts)
install(FILES ${core_gts_detail_headers} DESTINATION include/gts/detail)
install(FILES config.ini DESTINATION bin)

if (ENABLE_SSL)
	install(FILES gts_enable_ssl_p.h RENAME gts_enable_ssl.h DESTINATION include/gts)
else()
	install(FILES gts_disable_ssl_p.h RENAME gts_enable_ssl.h DESTINATION include/gts)
endif()

add_subdirectory(server)
add_subdirectory(http)
add_subdirectory(web)

option(ENABLE_DBI "-- gts: enable db interface" ON)
if (ENABLE_DBI)
	add_subdirectory(dbi)
endif()

option(ENABLE_UTLS "-- gts: enable utilities" ON)
if (ENABLE_UTLS)
	add_subdirectory(utls)
endif()
