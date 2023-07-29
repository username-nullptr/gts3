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

add_library(fmt SHARED ${fmt_sources} ${fmt_headers})

target_include_directories(fmt PRIVATE .)

set_target_properties(fmt PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS fmt DESTINATION bin)
install(FILES ${fmt_headers} DESTINATION include/fmt)
install(FILES cppformat DESTINATION include)
