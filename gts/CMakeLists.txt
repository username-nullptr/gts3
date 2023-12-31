project(GTS VERSION 3.0.0)

set(target_name gtscore)
add_definitions(-DGTS_VERSION_STR="3.0.0 dev6")

if (WIN32)
	set(OS_CPP win)
elseif (UNIX)
	set(OS_CPP unix)
endif()

set(core_sources
	global.cpp
	execution.cpp
	byte_order.cpp
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
	sha1.cpp
	log_buffer.cpp
	log.cpp
	coroutine.cpp
	coro_for_asio.cpp
	private/app_info.cpp
	private/application_${OS_CPP}.cpp
	private/tcp_socket_${OS_CPP}.cpp
)

set(core_gts_headers
	cplusplus.hpp
	cpp_attribute.h
	gts_namespace.h
	move_wrapper.h
	utility.h
	global.h
	execution.h
	byte_order.h
	value.h
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
	sha1.h
	chrono.h
	log_buffer.h
	log.h
	coroutine.h
	coro_for_asio.h
)

set(core_gts_detail_sources
	detail/coroutine_base.cpp
)

set(core_gts_detail_headers
	detail/move_wrapper.h
	detail/byte_order.h
	detail/value.h
	detail/application.h
	detail/settings.h
	detail/tcp_socket.h
	detail/registration.h
	detail/exception.h
	detail/rw_mutex.h
	detail/flags.h
	detail/log_buffer.h
	detail/log.h
	detail/function_traits.h
	detail/coro_utilities.h
	detail/coroutine_base.h
	detail/coroutine.h
	detail/coro_for_asio.h
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

set(all_files
	${core_sources}
	${core_gts_headers}
	${core_gts_detail_sources}
	${core_gts_detail_headers}
	${core_sp_headers}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_files})

add_library(${target_name} SHARED ${all_files})
target_compile_definitions(${target_name} PRIVATE ${target_name}_EXPORTS)

if (ENABLE_SSL)
	target_link_libraries(${target_name} PUBLIC ssl crypto)
endif()

target_link_libraries(${target_name} PUBLIC rttr_core copp)
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
