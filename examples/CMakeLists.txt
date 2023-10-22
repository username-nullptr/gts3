# resource
execute_process(COMMAND ${CMAKE_COMMAND} -E
	make_directory ${output_dir_name}/resource)

execute_process(COMMAND ${CMAKE_COMMAND} -E
	copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/web_resource ${output_dir_name}/resource)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES gtsstartupextensions.cpp)
add_library(gtsstartupextensions SHARED gtsstartupextensions.cpp)

target_include_directories(gtsstartupextensions PRIVATE ../src/core)
target_link_libraries(gtsstartupextensions gtscore)

set_target_properties(gtsstartupextensions PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../${output_dir_name}/bin/startup_extensions
)

add_subdirectory(web_business)

install(TARGETS gtsstartupextensions DESTINATION bin/startup_extensions)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/web_resource/ DESTINATION resource)
