# use asio timer
add_definitions(-DASIO_HAS_CHRONO)

set(gts_3rd_path ${CMAKE_SOURCE_DIR}/3rd_party)

include_directories(
	${gts_3rd_path}/rapidjson
	${gts_3rd_path}/inifile
	${gts_3rd_path}/asio
	${gts_3rd_path}/rttr
	${gts_3rd_path}/fmt
)

link_libraries(
	rttr_core
	fmt
)

if(UNIX)
	link_libraries(pthread)
endif()
