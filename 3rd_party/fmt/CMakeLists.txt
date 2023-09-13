if ((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND ${CMAKE_CXX_STANDARD} LESS 23) OR ${CMAKE_CXX_STANDARD} LESS 23)
	set(fmt_headers
		fmt/args.h
		fmt/color.h
		fmt/core.h
		fmt/format-inl.h
		fmt/ostream.h
		fmt/ranges.h
		fmt/xchar.h
		fmt/chrono.h
		fmt/compile.h
		fmt/format.h
		fmt/os.h
		fmt/printf.h
		fmt/std.h
	)
	set(fmt_sources
		fmt/format.cc
		fmt/os.cc
	)
	set(target_name fmt)

	add_library(${target_name} SHARED ${fmt_sources} ${fmt_headers})
	target_include_directories(${target_name} PRIVATE .)

	set_target_properties(${target_name} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
	)
	install(TARGETS ${target_name} DESTINATION bin)
	install(FILES ${fmt_headers} DESTINATION include/fmt)

	message("-- gts: c++ standard is smaller than 20/23 and uses a third-party format library")
endif()

install(FILES cppformat DESTINATION include)
