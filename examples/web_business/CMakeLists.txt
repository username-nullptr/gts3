add_library(plugin0 SHARED plugin0.cpp)
set_target_properties(plugin0 PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/business_plugins
)

add_library(plugin1 SHARED plugin1.cpp)
set_target_properties(plugin1 PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/business_plugins
)

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
	make_directory ${output_dir_name}/business_plugins)

execute_process(COMMAND ${CMAKE_COMMAND} -E
	copy ${CMAKE_CURRENT_SOURCE_DIR}/business_plugins.json ${output_dir_name}/business_plugins)

install(TARGETS cgi0 DESTINATION cgi)
install(TARGETS login DESTINATION cgi)
install(TARGETS upload DESTINATION cgi)

install(TARGETS plugin0 DESTINATION business_plugins)
install(TARGETS plugin1 DESTINATION business_plugins)
