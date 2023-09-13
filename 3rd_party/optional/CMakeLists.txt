if (${CMAKE_CXX_STANDARD} LESS 17)
	message("-- gts: c++ standard is smaller than 17 and uses a third-party optional library")
endif()

install(FILES cppoptional optional.hpp DESTINATION include)
