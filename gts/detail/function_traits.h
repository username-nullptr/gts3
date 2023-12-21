#ifndef GTS_DETAIL_FUNCTION_TRAITS_H
#define GTS_DETAIL_FUNCTION_TRAITS_H

#include <gts/utility.h>
#include <functional>

GTS_CORO_DETAIL_NAMESPACE_BEGIN

template<typename T>
struct is_function_ptr :
	std::integral_constant<bool,
		std::is_pointer<T>::value and
		std::is_function<typename std::remove_pointer<T>::type>::value
	> {};

struct operator_call_helper {
	void operator()(...) {}
};

template <typename T>
struct helper_composed: T, operator_call_helper {};

template <void (operator_call_helper::*)(...)>
struct member_function_holder {};

template <typename T, typename Ambiguous = member_function_holder<&operator_call_helper::operator()>>
struct is_functor_impl : std::true_type {};

template <typename T>
struct is_functor_impl<T, member_function_holder<&helper_composed<T>::operator()> > : std::false_type {};

template <typename T>
struct is_functor : std::conditional<std::is_class<T>::value, is_functor_impl<T>, std::false_type>::type {};

template<typename R, typename...Args>
struct is_functor<R(*)(Args...)> : std::true_type {};

template<typename R, typename... Args>
struct is_functor<R (&)(Args...)> : std::true_type {};

#if GTS_CPLUSPLUS >= 201703L
template<typename R, typename... Args>
struct is_functor<R(*)(Args...) noexcept> : std::true_type {};

template<typename R, typename... Args>
struct is_functor<R(&)(Args...) noexcept> : std::true_type {};
#endif

template <typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

template<typename R, typename...Args>
struct function_traits<R(Args...)>
{
	static constexpr std::size_t arg_count = sizeof...(Args);
	using return_type = R;
	using arg_types = std::tuple<Args...>;
};

template<typename R, typename...Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};

template<typename R, typename...Args>
struct function_traits<R(&)(Args...)> : function_traits<R(Args...)> {};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)> {
	using class_type = C;
};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R (Args...)> {
	using class_type = C;
};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...) volatile> : function_traits<R(Args...)> {
	using class_type = C;
};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...) const volatile> : function_traits<R(Args...)> {
	using class_type = C;
};

#if GTS_CPLUSPLUS >= 201703L
template<typename R, typename...Args>
struct function_traits<R(*)(Args...) noexcept> : function_traits<R(Args...)> {};

template<typename R, typename...Args>
struct function_traits<R(&)(Args...) noexcept> : function_traits<R(Args...)> {};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...) noexcept> : function_traits<R(Args...)> {
	using class_type = C;
};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...) const noexcept> : function_traits<R(Args...)> {
	using class_type = C;
};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...) volatile noexcept> : function_traits<R(Args...)> {
	using class_type = C;
};

template<typename R, typename C, typename...Args>
struct function_traits<R(C::*)(Args...) const volatile noexcept> : function_traits<R(Args...)> {
	using class_type = C;
};
#endif

template<typename T>
struct function_traits<std::function<T>> : function_traits<T> {};

template<typename F, std::size_t Index>
struct param_types {
	using type = tuple_element_t<Index, typename function_traits<F>::arg_types>;
};

template<typename F, std::size_t Index>
using param_types_t = typename param_types<F, Index>::type;

template<typename F>
struct is_void_func : std::conditional<
	std::is_same<typename function_traits<F>::return_type, void>::value,
	std::true_type, std::false_type
>::type {};

template<typename F>
using is_function = std::integral_constant<bool,
	std::is_member_function_pointer<F>::value or
	std::is_function<F>::value or
	is_functor<F>::value
>;

GTS_CORO_DETAIL_NAMESPACE_END


#endif //GTS_DETAIL_FUNCTION_TRAITS_H
