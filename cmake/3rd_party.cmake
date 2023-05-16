# use asio timer
add_definitions(-DASIO_HAS_CHRONO)

set(gts_3rd_path ${CMAKE_SOURCE_DIR}/3rd_party)

if (${CMAKE_CXX_STANDARD} LESS 17)
	message("-- gts: c++ standard is smaller than 17 and uses a third-party filesystem library")
endif()

include_directories(
	${gts_3rd_path}/filesystem
	${gts_3rd_path}/rapidjson
	${gts_3rd_path}/inifile
	${gts_3rd_path}/asio
	${gts_3rd_path}/rttr
	${gts_3rd_path}/fmt
)

link_libraries(fmt)

if(UNIX)
	link_libraries(pthread)
endif()
