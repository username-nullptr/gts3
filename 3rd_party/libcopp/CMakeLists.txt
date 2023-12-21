set(target_name copp)

if (UNIX)
	set(OS_STD posix)
else ()
	set(OS_STD windows)
endif()

set(utils_sources
	libcopp/utils/uint64_id_allocator.cpp
)

set(utils_headers
	libcopp/utils/atomic_int_type.h
	libcopp/utils/errno.h
	libcopp/utils/features.h
	libcopp/utils/intrusive_ptr.h
	libcopp/utils/lock_holder.h
	libcopp/utils/spin_lock.h
	libcopp/utils/uint64_id_allocator.h
	libcopp/utils/config/compile_optimize.h
	libcopp/utils/config/libcopp_build_features.h
	libcopp/utils/config/stl_include_prefix.h
	libcopp/utils/config/stl_include_suffix.h
	libcopp/utils/std/coroutine.h
	libcopp/utils/std/explicit_declare.h
	libcopp/utils/std/type_traits.h
)

set(stack_sources
	libcopp/stack/stack_context.cpp
	libcopp/stack/allocator/stack_allocator_malloc.cpp
	libcopp/stack/allocator/stack_allocator_memory.cpp
	libcopp/stack/allocator/stack_allocator_${OS_STD}.cpp
	libcopp/stack/stack_traits/stack_traits_${OS_STD}.cpp
)

set(stack_headers
	libcopp/stack/stack_allocator.h
	libcopp/stack/stack_context.h
	libcopp/stack/stack_pool.h
	libcopp/stack/stack_traits.h
	libcopp/stack/allocator/stack_allocator_malloc.h
	libcopp/stack/allocator/stack_allocator_memory.h
	libcopp/stack/allocator/stack_allocator_pool.h
	libcopp/stack/allocator/stack_allocator_${OS_STD}.h
)

set(fcontext_srouces
)
set(fcontext_headers
	libcopp/fcontext/all.hpp
	libcopp/fcontext/fcontext.hpp
	libcopp/fcontext/detail/config.hpp
)

set(coro_sources
	libcopp/coroutine/coroutine_context.cpp
	libcopp/coroutine/std_coroutine_common.cpp
)

set(coro_headers
	libcopp/coroutine/algorithm.h
	libcopp/coroutine/algorithm_common.h
	libcopp/coroutine/callable_promise.h
	libcopp/coroutine/coroutine_context.h
	libcopp/coroutine/coroutine_context_base.h
	libcopp/coroutine/coroutine_context_container.h
	libcopp/coroutine/generator_promise.h
	libcopp/coroutine/std_coroutine_common.h
)

if (BORLAND)
	list(APPEND utils_headers
		libcopp/utils/abi/borland_prefix.hpp
		libcopp/utils/abi/borland_suffix.hpp
	)
elseif (MSVC)
	list(APPEND utils_headers
		libcopp/utils/abi/msvc_prefix.hpp
		libcopp/utils/abi/msvc_suffix.hpp
	)
endif()

if (LIBCOPP_MACRO_USE_SEGMENTED_STACKS)
	list(APPEND stack_sources
		libcopp/stack/allocator/stack_allocator_split_segment.cpp
	)
	list(APPEND stack_headers
		libcopp/stack/allocator/stack_allocator_split_segment.h
	)
endif()

include(libcopp/fcontext/detect/fcontext.detect.cmake)

set(fcontext_bin_name_make fcontext_make.${fcontext_as_output_suffix})
set(fcontext_bin_name_jump fcontext_jump.${fcontext_as_output_suffix})
set(fcontext_bin_name_ontop fcontext_ontop.${fcontext_as_output_suffix})

set(fcontext_src_name_make
	make_${os_platform}_${fcontext_abi}_${bin_format}_${fcontext_as_tool}
)
set(fcontext_src_name_jump
	jump_${os_platform}_${fcontext_abi}_${bin_format}_${fcontext_as_tool}
)
set(fcontext_src_name_ontop
	ontop_${os_platform}_${fcontext_abi}_${bin_format}_${fcontext_as_tool}
)

enable_language(ASM)
include(libcopp/fcontext/tools/${fcontext_as_tool}.cmake)

set(all_files
	${utils_sources}
	${utils_headers}
	${stack_headers}
	${stack_sources}
	${fcontext_sources}
	${fcontext_headers}
	${coro_sources}
	${coro_headers}
)
add_library(${target_name} SHARED ${all_files})

target_compile_definitions(${target_name} PRIVATE libcopp_EXPORTS)
target_include_directories(${target_name} PRIVATE .)

if (UNIX)
	target_link_libraries(${target_name} PRIVATE pthread)
endif ()

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)
install(TARGETS ${target_name} DESTINATION bin)

install(DIRECTORY libcopp
	DESTINATION include
	FILES_MATCHING PATTERN "*.h*"
)