set(server_sources
	main.cpp
	application.cpp
	cmdline/arguments.cpp
	cmdline/app_core.cpp
	cmdline/interaction.cpp
	server/tcp_server.cpp
	server/cmdline_server.cpp
	subserver/server_ope.cpp
	subserver/subserver_manager.cpp
)

set(server_headers
	global.h
	application.h
	gts_plugin.h
	cmdline/arguments.h
	cmdline/app_core.h
	cmdline/interaction.h
	cmdline/private/app_core_p.h
	cmdline/private/interaction_p.h
	server/tcp_server.h
	server/cmdline_server.h
	subserver/server_ope.h
	subserver/subserver_manager.h
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

add_executable(gts ${server_sources} ${server_headers})

target_include_directories(gts PRIVATE .)
target_include_directories(gts PRIVATE ../core)
target_include_directories(gts PRIVATE ../log)

target_link_libraries(gts PRIVATE gtscore)
target_link_libraries(gts PRIVATE gtslog)

set_target_properties(gts PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
