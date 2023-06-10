# resource
execute_process(COMMAND ${CMAKE_COMMAND} -E
	make_directory ${output_dir_name}/resource)

execute_process(COMMAND ${CMAKE_COMMAND} -E
	copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/web_resource ${output_dir_name}/resource)

add_library(gtsstartupextensions SHARED gtsstartupextensions.cpp)

set_target_properties(gtsstartupextensions PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../${output_dir_name}/bin
)

add_subdirectory(web_business)

install(TARGETS gtsstartupextensions DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/web_resource/ DESTINATION resource)
