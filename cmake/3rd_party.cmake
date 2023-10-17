set(gts_3rd_path ${CMAKE_SOURCE_DIR}/3rd_party)

include_directories(
	${gts_3rd_path}/nlohmann.json
	${gts_3rd_path}/filesystem
	${gts_3rd_path}/optional
	${gts_3rd_path}/inifile
	${gts_3rd_path}/asio
	${gts_3rd_path}/rttr
	${gts_3rd_path}/fmt
)

link_libraries(fmt)
