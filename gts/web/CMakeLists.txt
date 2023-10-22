set(target_name gtsweb)

set(web_sources
	main.cpp
	socket.cpp
	registration.cpp
	service/connection.cpp
	service/task.cpp
	service/service_io.cpp
	service/plugins_service.cpp
	service/cgi/service.cpp
	service/cgi/process.cpp
	service/cgi/process_${OS_CPP}.cpp
)

set(web_gts_headers
	global.h
	types.h
	socket.h
	config_key.h
	thread_pool.h
	registration.h
)

set(web_gts_detail_headers
	detail/registration.h
	detail/socket.h
	detail/types.h
)

set(web_sp_headers
	service/connection.h
	service/task.h
	service/service_io.h
	service/plugins_service.h
	service/cgi/service.h
	service/cgi/process.h
	service/cgi/process_p.h
)

set(all_files
	${web_sources}
	${web_gts_headers}
	${web_gts_detail_headers}
	${web_sp_headers}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_files})
add_library(${target_name} SHARED ${all_files})

target_compile_definitions(${target_name} PRIVATE ${target_name}_EXPORTS)
target_link_libraries(${target_name} PUBLIC gtshttp)

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)
install(FILES ${web_gts_headers} DESTINATION include/gts/web)
install(FILES ${web_gts_detail_headers} DESTINATION include/gts/web/detail)
