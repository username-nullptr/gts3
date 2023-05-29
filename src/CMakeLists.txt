project(GTS VERSION 3.0.0)
add_compile_definitions(GTS_VERSION_STR="3.0.0-dev")

if (ENABLE_SSL AND SSL_DIR)
	include_directories(${SSL_DIR}/include)
endif()

add_subdirectory(core)
add_subdirectory(log)
add_subdirectory(server)
add_subdirectory(web)

install(FILES config.ini DESTINATION bin)
install(FILES sites.json.in RENAME sites.json DESTINATION bin)
