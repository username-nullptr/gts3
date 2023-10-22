set(target_name gts)

set(server_sources
	main.cpp
	server_tool.cpp
	application.cpp
	cmdline/arguments.cpp
	cmdline/app_core.cpp
	cmdline/interaction.cpp
	server/tcp_server.cpp
	server/cmdline_server.cpp
#	subserver/server_ope.cpp
#	subserver/subserver_manager.cpp
)

set(server_headers
	global.h
	server_tool.h
	application.h
	cmdline/arguments.h
	cmdline/app_core.h
	cmdline/interaction.h
	server/tcp_server.h
	server/cmdline_server.h
#	subserver/server_ope.h
#	subserver/subserver_manager.h
)

set(server_private_headers
	cmdline/private/app_core_p.h
	cmdline/private/interaction_p.h
)

if(WIN32)
	list(APPEND server_sources
		cmdline/private/app_core_win.cpp
		cmdline/private/interaction_win.cpp
	)
elseif(UNIX)
	list(APPEND server_sources
		cmdline/private/app_core_unix.cpp
		cmdline/private/interaction_unix.cpp
	)
endif()

set(all_files ${target_name} ${server_sources} ${server_headers} ${server_private_headers})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_files})
add_executable(${all_files})

target_include_directories(${target_name} PRIVATE .)
target_link_libraries(${target_name} PRIVATE gtscore)

set_target_properties(${target_name} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)
install(FILES sites.json.in RENAME sites.json DESTINATION bin)
