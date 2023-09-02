# plugin service

add_library(plugin0 SHARED plugin0.cpp)
target_include_directories(plugin0 PRIVATE ../../src/core)
target_include_directories(plugin0 PRIVATE ../../src/log)
target_include_directories(plugin0 PRIVATE ../../src/web)
target_link_libraries(plugin0 PRIVATE gtsweb)
set_target_properties(plugin0 PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/plugins
)

add_library(plugin1 SHARED plugin1.cpp)
target_include_directories(plugin1 PRIVATE ../../src/core)
target_include_directories(plugin1 PRIVATE ../../src/log)
target_include_directories(plugin1 PRIVATE ../../src/web)
target_link_libraries(plugin1 gtsweb)
set_target_properties(plugin1 PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/plugins
)

add_library(plugin2 SHARED plugin2.cpp)
target_include_directories(plugin2 PRIVATE ../../src/core)
target_include_directories(plugin2 PRIVATE ../../src/log)
target_include_directories(plugin2 PRIVATE ../../src/web)
target_link_libraries(plugin2 gtsweb)
set_target_properties(plugin2 PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/plugins
)

# cgi service

add_executable(cgi0 cgi0.cpp)
set_target_properties(cgi0 PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ../../${output_dir_name}/cgi
)

add_executable(login login.cpp)
set_target_properties(login PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ../../${output_dir_name}/cgi
)

add_executable(upload upload.cpp)
set_target_properties(upload PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ../../${output_dir_name}/cgi
)

execute_process(COMMAND ${CMAKE_COMMAND} -E
	make_directory ${output_dir_name}/plugins)

execute_process(COMMAND ${CMAKE_COMMAND} -E
	copy ${CMAKE_CURRENT_SOURCE_DIR}/plugins.json ${output_dir_name})

install(TARGETS cgi0 DESTINATION cgi)
install(TARGETS login DESTINATION cgi)
install(TARGETS upload DESTINATION cgi)

install(TARGETS plugin0 DESTINATION plugins)
install(TARGETS plugin1 DESTINATION plugins)
install(TARGETS plugin2 DESTINATION plugins)
