function(get_latest_supported_cxx CXX_STANDARD)
	if (POLICY CMP0067)
		cmake_policy(SET CMP0067 NEW)
	endif()

	if(${CMAKE_VERSION} VERSION_LESS "3.8.0")
		set(CMAKE_CXX_STANDARD 14)
	else()
		set(CMAKE_CXX_STANDARD 20)
	endif()

	include(CheckCXXSourceCompiles)

	check_cxx_source_compiles("
							  #include <type_traits>
							  typedef void F();
							  typedef void G() noexcept;

							  std::enable_if<
								  !std::is_same<F, G>::value,
								  int
							  >::type i = 42;

							  int main() { return 0; }
							  "
							  HAS_NO_EXCEPT_TYPE_SIGNATURE_SUPPORT)

	check_cxx_source_compiles("
							  #include <type_traits>
							  struct foo { void func() const noexcept {} };
							  template<typename T>
							  void test_func(T)
							  {
								  static_assert(std::is_member_function_pointer<T>::value, \"Failed\");
							  }
							  int main() { test_func(&foo::func); return 0; }
							  "
							  HAS_STL_NO_EXCEPT_TYPE_SIGNATURE_SUPPORT)

	check_cxx_source_compiles("
							  constexpr int abs(int x)
							  {
								  if(x < 0) x = -x;
								  return x;
							  }

							  int main() { }
							  "
							  HAS_CXX_CONSTEXPR)

	check_cxx_source_compiles( "
							   #include <type_traits>
							   template<typename T>
							   struct template_type_trait : std::false_type {};

							   template<template < bool > class T, bool N>
							   struct template_type_trait<T<N>> : std::true_type {};

							   template<template <std::size_t> class T, std::size_t N>
							   struct template_type_trait<T<N>> : std::true_type {};

							   template<std::size_t T>
							   struct bar{};

							   int main() { static bool foo = template_type_trait<bar<100>>::value;}
							   "
							   HAS_PARTIAL_SPECIALIZATION_FOR_ARRAYS)

	check_cxx_source_compiles( "
							   class A {
							   public: int a:4 = 2;
							   };
							   int main() { A a; return 0; }
							   "
							   _HAS_BIT_FIELD_INITIALIZATION)

	check_cxx_source_compiles( "
							   class A {
							   public:
							   int a = 0;
							   bool operator<=>(const A&) const = default;
							   };
							   int main() { A a; return 0; }
							   "
							   _HAS_THREE_TERM_OPERATOR)

	check_cxx_source_compiles( "
							   auto func(auto a) { return a; }
							   int main() { func(1); return 0; }
							   "
							   HAS_AUTO_TEMPLATE)

	if (_HAS_BIT_FIELD_INITIALIZATION AND _HAS_THREE_TERM_OPERATOR AND HAS_AUTO_TEMPLATE)
		set(MAX_CXX_STD 20)
	elseif (HAS_NO_EXCEPT_TYPE_SIGNATURE_SUPPORT AND HAS_STL_NO_EXCEPT_TYPE_SIGNATURE_SUPPORT AND
		HAS_PARTIAL_SPECIALIZATION_FOR_ARRAYS)
		set(MAX_CXX_STD 17)
	else()
		if (HAS_CXX_CONSTEXPR)
			set(MAX_CXX_STD 14)
		else()
			set(MAX_CXX_STD 11)
		endif()
	endif()

	set(${CXX_STANDARD} ${MAX_CXX_STD} PARENT_SCOPE)
endfunction()
