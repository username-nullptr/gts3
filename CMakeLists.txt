cmake_minimum_required(VERSION 3.1)

project(GTS)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(utility)
get_latest_supported_cxx(CXX_STANDARD)

set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
message("--")
message("-- gts: using C++: " ${CMAKE_CXX_STANDARD})
message("-- gts: build type: " ${CMAKE_BUILD_TYPE})
message("-- gts: install prefix: " ${CMAKE_INSTALL_PREFIX})

set(output_dir_name output)

add_subdirectory(3rd_party)
include(3rd_party)

option(ENABLE_SSL "-- gts: enable link libssl" OFF)
if (ENABLE_SSL)
	message("-- gts: enable open ssl. (the version must be greater than or equal to 1.0.2)")
	add_definitions(-D__ENABLE_SSL__)
	if (OpenSSL_DIR)
		message("-- gts: manually specify the path of openssl: ${OpenSSL_DIR}")
		include_directories(${OpenSSL_DIR}/include)
		link_directories(${OpenSSL_DIR}/lib)
	else()
		find_package(OpenSSL 1.0.2 REQUIRED)
	endif()
endif()

if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	add_definitions(-DGTS_DEBUG)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if (CMAKE_CXX_STANDARD GREATER 14 AND CMAKE_CXX_STANDARD LESS 23)
		link_libraries(stdc++fs)
	endif()
endif()

add_compile_options(-Wall)

include_directories(.)
add_subdirectory(gts)

option(BUILD_EXAMPLES "-- gts: enable this to build the examples" ON)

if (BUILD_EXAMPLES)
	message("-- gts: enable this to build the examples.")
	add_subdirectory(examples)
	install(FILES examples/web_business/plugins.json DESTINATION .)
else()
	install(FILES gts/server/plugins.json.in RENAME plugins.json DESTINATION .)
endif()