project(GTS VERSION 3.0.0)
add_compile_definitions(GTS_VERSION_STR="3.0.0")

if (CMKE_SIZEOF_VOID_P EQUAL 8)
	set(lib_dir_suffix 64)
	message("-- gts: system: 64bit")
else()
	message("-- gts: system: 32bit")
endif()

if (ENABLE_SSL AND SSL_DIR)
	include_directories(${SSL_DIR}/include)
endif()

add_subdirectory(core)
add_subdirectory(log)
add_subdirectory(server)
add_subdirectory(web)

install(FILES config.ini DESTINATION bin)
