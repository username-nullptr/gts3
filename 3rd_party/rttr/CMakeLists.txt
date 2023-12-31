set(target_name rttr_core)

set(rttr_headers
	rttr/access_levels.h
	rttr/argument.h
	rttr/array_range.h
	rttr/associative_mapper.h
	rttr/constructor.h
	rttr/destructor.h
	rttr/enumeration.h
	rttr/enum_flags.h
	rttr/filter_item.h
	rttr/instance.h
	rttr/library.h
	rttr/method.h
	rttr/policy.h
	rttr/property.h
	rttr/parameter_info.h
	rttr/registration
	rttr/registration_friend
	rttr/registration.h
	rttr/registration_friend.h
	rttr/sequential_mapper.h
	rttr/string_view.h
	rttr/rttr_cast.h
	rttr/rttr_enable.h
	rttr/type
	rttr/type.h
	rttr/variant.h
	rttr/variant_associative_view.h
	rttr/variant_sequential_view.h
	rttr/wrapper_mapper.h
	rttr/detail/base/core_prerequisites.h
	rttr/detail/base/version.h
	rttr/detail/comparison/comparable_types.h
	rttr/detail/comparison/compare_array_less.h
	rttr/detail/comparison/compare_array_less_impl.h
	rttr/detail/comparison/compare_array_equal.h
	rttr/detail/comparison/compare_array_equal_impl.h
	rttr/detail/comparison/compare_equal.h
	rttr/detail/comparison/compare_equal_impl.h
	rttr/detail/comparison/compare_less.h
	rttr/detail/comparison/compare_less_impl.h
	rttr/detail/constructor/constructor_invoker.h
	rttr/detail/constructor/constructor_wrapper.h
	rttr/detail/constructor/constructor_wrapper_defaults.h
	rttr/detail/constructor/constructor_wrapper_base.h
	rttr/detail/conversion/std_conversion_functions.h
	rttr/detail/conversion/number_conversion.h
	rttr/detail/default_arguments/default_arguments.h
	rttr/detail/default_arguments/invoke_with_defaults.h
	rttr/detail/destructor/destructor_wrapper.h
	rttr/detail/destructor/destructor_wrapper_base.h
	rttr/detail/enumeration/enumeration_helper.h
	rttr/detail/enumeration/enumeration_wrapper.h
	rttr/detail/enumeration/enumeration_wrapper_base.h
	rttr/detail/enumeration/enum_data.h
	rttr/detail/filter/filter_item_funcs.h
	rttr/detail/impl/argument_impl.h
	rttr/detail/impl/array_range_impl.h
	rttr/detail/impl/associative_mapper_impl.h
	rttr/detail/impl/enum_flags_impl.h
	rttr/detail/impl/instance_impl.h
	rttr/detail/impl/rttr_cast_impl.h
	rttr/detail/impl/sequential_mapper_impl.h
	rttr/detail/impl/string_view_impl.h
	rttr/detail/impl/wrapper_mapper_impl.h
	rttr/detail/metadata/metadata.h
	rttr/detail/metadata/metadata_handler.h
	rttr/detail/method/method_accessor.h
	rttr/detail/method/method_invoker.h
	rttr/detail/method/method_wrapper.h
	rttr/detail/method/method_wrapper_base.h
	rttr/detail/misc/argument_wrapper.h
	rttr/detail/misc/argument_extractor.h
	rttr/detail/misc/class_item_mapper.h
	rttr/detail/misc/data_address_container.h
	rttr/detail/misc/flat_map.h
	rttr/detail/misc/flat_multimap.h
	rttr/detail/misc/function_traits.h
	rttr/detail/misc/iterator_wrapper.h
	rttr/detail/misc/misc_type_traits.h
	rttr/detail/misc/register_wrapper_mapper_conversion.h
	rttr/detail/misc/sequential_container_type_traits.h
	rttr/detail/misc/std_type_traits.h
	rttr/detail/misc/template_type_trait.h
	rttr/detail/misc/template_type_trait_impl.h
	rttr/detail/misc/utility.h
	rttr/detail/parameter_info/parameter_infos.h
	rttr/detail/parameter_info/parameter_infos_compare.h
	rttr/detail/parameter_info/parameter_names.h
	rttr/detail/parameter_info/parameter_info_wrapper_base.h
	rttr/detail/parameter_info/parameter_info_wrapper.h
	rttr/detail/policies/prop_policies.h
	rttr/detail/policies/meth_policies.h
	rttr/detail/policies/ctor_policies.h
	rttr/detail/property/property_accessor.h
	rttr/detail/property/property_wrapper.h
	rttr/detail/property/property_wrapper_base.h
	rttr/detail/property/property_wrapper_func.h
	rttr/detail/property/property_wrapper_member_func.h
	rttr/detail/property/property_wrapper_member_object.h
	rttr/detail/property/property_wrapper_object.h
	rttr/detail/registration/bind_types.h
	rttr/detail/registration/bind_impl.h
	rttr/detail/registration/register_base_class_from_accessor.h
	rttr/detail/registration/registration_impl.h
	rttr/detail/registration/registration_executer.h
	rttr/detail/registration/registration_manager.h
	rttr/detail/registration/registration_state_saver.h
	rttr/detail/type/accessor_type.h
	rttr/detail/type/base_classes.h
	rttr/detail/type/get_create_variant_func.h
	rttr/detail/type/get_derived_info_func.h
	rttr/detail/type/type_converter.h
	rttr/detail/type/type_comparator.h
	rttr/detail/type/type_data.h
	rttr/detail/type/type_register.h
	rttr/detail/type/type_impl.h
	rttr/detail/type/type_name.h
	rttr/detail/type/type_string_utils.h
	rttr/detail/variant/variant_compare.h
	rttr/detail/variant/variant_data.h
	rttr/detail/variant/variant_data_converter.h
	rttr/detail/variant/variant_data_policy.h
	rttr/detail/variant/variant_impl.h
	rttr/detail/variant_associative_view/variant_associative_view_private.h
	rttr/detail/variant_associative_view/variant_associative_view_creator.h
	rttr/detail/variant_associative_view/variant_associative_view_creator_impl.h
	rttr/detail/variant_sequential_view/variant_sequential_view_private.h
	rttr/detail/variant_sequential_view/variant_sequential_view_creator_impl.h
	rttr/detail/variant_sequential_view/variant_sequential_view_creator.h
)

set(rttr_private_headers
	rttr/detail/library/library_p.h
	rttr/detail/type/type_register_p.h
)

set(rttr_sources
	rttr/constructor.cpp
	rttr/destructor.cpp
	rttr/enumeration.cpp
	rttr/library.cpp
	rttr/method.cpp
	rttr/parameter_info.cpp
	rttr/policy.cpp
	rttr/property.cpp
	rttr/registration.cpp
	rttr/type.cpp
	rttr/variant.cpp
	rttr/variant_associative_view.cpp
	rttr/variant_sequential_view.cpp
	rttr/detail/comparison/compare_equal.cpp
	rttr/detail/comparison/compare_less.cpp
	rttr/detail/misc/standard_types.cpp
	rttr/detail/conversion/std_conversion_functions.cpp
	rttr/detail/constructor/constructor_wrapper_base.cpp
	rttr/detail/destructor/destructor_wrapper_base.cpp
	rttr/detail/enumeration/enumeration_helper.cpp
	rttr/detail/enumeration/enumeration_wrapper_base.cpp
	rttr/detail/method/method_wrapper_base.cpp
	rttr/detail/parameter_info/parameter_info_wrapper_base.cpp
	rttr/detail/property/property_wrapper_base.cpp
	rttr/detail/registration/registration_executer.cpp
	rttr/detail/registration/registration_state_saver.cpp
	rttr/detail/type/type_data.cpp
	rttr/detail/type/type_register.cpp
	rttr/detail/variant/variant_compare.cpp
)

if (WIN32)
	list(APPEND rttr_sources
		rttr/detail/library/library_win.cpp
	)
elseif (UNIX)
	list(APPEND rttr_sources
		rttr/detail/library/library_unix.cpp
	)
endif()

add_library(${target_name} SHARED ${rttr_sources} ${rttr_headers} ${rttr_private_headers})
target_compile_definitions(${target_name} PRIVATE RTTR_DLL RTTR_DLL_EXPORTS)

if (${CMAKE_CXX_STANDARD} GREATER 17)
	set_target_properties(${target_name} PROPERTIES CXX_STANDARD 17)
endif()

if (UNIX)
	target_link_libraries(${target_name} PRIVATE dl)
endif()

target_include_directories(${target_name} PRIVATE .)

set_target_properties(${target_name} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ../../${output_dir_name}/bin
)

install(TARGETS ${target_name} DESTINATION bin)

install(DIRECTORY rttr
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
	PATTERN "*_p.h" EXCLUDE
)
install(FILES rttr/type rttr/registration DESTINATION include/rttr)
