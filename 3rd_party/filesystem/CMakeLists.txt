if (${CMAKE_CXX_STANDARD} LESS 17)
	message("-- gts: c++ standard is smaller than 17 and uses a third-party filesystem library")
endif()

install(FILES cppfilesystem filesystem.hpp DESTINATION include)
