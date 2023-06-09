cmake_minimum_required(VERSION 3.16)

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

if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	add_definitions(-DGTS_DEBUG)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if (${CMAKE_CXX_STANDARD} GREATER 14)
		link_libraries(stdc++fs)
	endif()
endif()

option(ENABLE_SSL "-- gts: enable link libssl" ON)

add_definitions(-DUSING_ASIO)
add_compile_options(-Wall)
add_subdirectory(src)

option(BUILD_EXAMPLES "-- gts: enable this to build the examples" ON)

if (BUILD_EXAMPLES)
	message("-- gts: enable this to build the examples.")
	add_subdirectory(examples)
endif()

install(FILES examples/web_business/business_plugins.json DESTINATION business_plugins)
