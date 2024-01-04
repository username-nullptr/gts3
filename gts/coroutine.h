#ifndef GTS_COROUTINE_H
#define GTS_COROUTINE_H

#include <gts/detail/coroutine_base.h>

GTS_NAMESPACE_BEGIN

template <typename Ret, typename Arg0, typename...Args>
class coroutine<std::function<Ret(Arg0,Args...)>> :
	public coro_detail::coroutine_base_return<Ret>,
	public coro_detail::coroutine_base_args<Arg0,Args...>
{
	GTS_DISABLE_COPY(coroutine)
	coroutine() = default;

	using base_type = coro_detail::coroutine_base_args<Arg0,Args...>;
	using this_type = coroutine<std::function<Ret(Arg0,Args...)>>;
	using this_ptr_type = coroutine_ptr<Ret(Arg0,Args...)>;

public:
	template <std::size_t...I> static this_ptr_type create
	(std::function<Ret(Arg0,Args...)> func, std::size_t stack_size, coro_detail::index_sequence<I...>);

public:
public:
	coroutine(coroutine&&) noexcept = default;
	coroutine &operator=(coroutine&&) noexcept = default;
};

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Ret>
class coroutine<std::function<Ret()>> :
	public coro_detail::coroutine_base_return<Ret>,
	public coro_detail::coroutine_base
{
	GTS_DISABLE_COPY(coroutine)
	coroutine() = default;

	using base_type = coro_detail::coroutine_base;
	using this_type = coroutine<std::function<Ret()>>;
	using this_ptr_type = coroutine_ptr<Ret()>;

public:
	static this_ptr_type create(std::function<Ret()> func, std::size_t stack_size);

public:
public:
	coroutine(coroutine&&) noexcept = default;
	coroutine &operator=(coroutine&&) noexcept = default;
};

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Arg0, typename...Args>
class coroutine<std::function<void(Arg0,Args...)>> :
	public coro_detail::coroutine_base_return<void>,
	public coro_detail::coroutine_base_args<Arg0,Args...>
{
	GTS_DISABLE_COPY(coroutine)
	coroutine() = default;

	using base_type = coro_detail::coroutine_base_args<Arg0,Args...>;
	using this_type = coroutine<std::function<void(Arg0,Args...)>>;
	using this_ptr_type = coroutine_ptr<void(Arg0,Args...)>;

public:
	template <std::size_t...I>
	static this_ptr_type create
	(std::function<void(Arg0,Args...)> func, std::size_t stack_size, coro_detail::index_sequence<I...>);

public:
public:
	coroutine(coroutine&&) noexcept = default;
	coroutine &operator=(coroutine&&) noexcept = default;
};

/*--------------------------------------------------------------------------------------------------------------------*/

template <>
class coroutine<std::function<void()>> :
	public coro_detail::coroutine_base_return<void>,
	public coro_detail::coroutine_base
{
	GTS_DISABLE_COPY(coroutine)
	coroutine() = default;

	using base_type = coro_detail::coroutine_base;
	using this_type = coroutine<std::function<void()>>;
	using this_ptr_type = coroutine_ptr<void()>;

public:
	static this_ptr_type create(std::function<void()> func, std::size_t stack_size);

public:
	coroutine(coroutine&&) noexcept = default;
	coroutine &operator=(coroutine&&) noexcept = default;
};

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Func, typename FT = coro_detail::function_traits<Func>, typename U0 =
	typename std::enable_if <
	    FT::arg_count == 0, int
	>::type
>
auto create_coroutine(Func &&func, std::size_t stack_size = 0x7FFFF)
->
coroutine_ptr<typename FT::return_type()>
{
	static_assert(coro_detail::is_function<Func>::value, "The template type must be a callable type.");
	using return_type = typename FT::return_type;

	using coro_t = coroutine<std::function<return_type()>>;
	return coro_t::create(std::forward<Func>(func), stack_size);
}

template <typename Func, typename FT = coro_detail::function_traits<Func>, typename U0 =
	typename std::enable_if <
	    FT::arg_count >= 1, int
	>::type
>
auto create_coroutine(Func &&func, std::size_t stack_size = 0x7FFFF)
->
decltype ( // coroutine_ptr<return_type(void)>
	coro_detail::create_coroutine (
		std::forward<Func>(func),
		stack_size,
		coro_detail::make_index_sequence<FT::arg_count>{}
	)
){
	static_assert(coro_detail::is_function<Func>::value, "The template type must be a callable type.");
	return coro_detail::create_coroutine
		(std::forward<Func>(func), stack_size, coro_detail::make_index_sequence<FT::arg_count>{});
}

template <typename Func, typename...Args>
auto start_coroutine(Func &&func, std::size_t stack_size = 0x7FFFF)
->
decltype(create_coroutine(std::forward<Func>(func), stack_size));

/*--------------------------------------------------------------------------------------------------------------------*/

using this_coro = coro_detail::this_coro;

using coro_context = coro_detail::coro_context;

#define GTS_CORO_FUNC

/*--------------------------------------------------------------------------------------------------------------------*/

void coro_yield();

template <typename Func, typename...Args>
void coro_yield(Func &&func, Args&&...args);

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Ret, typename...Args>
Ret coro_await(coroutine_ptr<Ret(Args...)> coro, Args...args);

template <typename Ret, typename...Args>
Ret coro_await(coroutine<std::function<Ret(Args...)>> &coro, Args...args);

template <typename Func, typename...Args>
enable_if_t <
    coro_detail::is_functor<Func>::value,
	decltype(std::declval<Func>()(std::declval<Args>()...))
>
coro_await(Func &&func, Args&&...args, std::size_t stack_size = 0x7FFFF);

GTS_NAMESPACE_END
#include <gts/detail/coroutine.h>


#endif //GTS_COROUTINE_H
