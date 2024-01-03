#ifndef GTS_DETAIL_COROUTINE_H
#define GTS_DETAIL_COROUTINE_H

#include <asio.hpp>

GTS_NAMESPACE_BEGIN

template <typename Ret, typename Arg0, typename...Args>
template <std::size_t...I>
coroutine_ptr<Ret(Arg0,Args...)>
coroutine<std::function<Ret(Arg0,Args...)>>::
create(std::function<Ret(Arg0,Args...)> func, std::size_t stack_size, coro_detail::index_sequence<I...>)
{
	auto self = std::shared_ptr<this_type>(new this_type());
	self->m_coro = base_type::coro_context::create(std::function<int(void*)>([self, func](void *data)
	{
		if( data == nullptr )
			throw exception("coroutine<...>: No arguments were passed for the first invoke.");

		auto t = static_cast<std::tuple<Arg0,Args...>*>(data);
		self->save_return(func(std::get<I>(*t)...));

		self->_finished();
		return 0;
	}),
	stack_size);
	return self;
}

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Ret>
coroutine_ptr<Ret()>
coroutine<std::function<Ret()>>::
create(std::function<Ret()> func, std::size_t stack_size)
{
	auto self = std::shared_ptr<this_type>(new this_type());
	self->m_coro = base_type::coro_context::create(std::function<int(void*)>([self, func](void*)
	{
		self->save_return(func());
		self->_finished();
		return 0;
	}),
	stack_size);
	return self;
}

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Arg0, typename...Args>
template <std::size_t...I>
coroutine_ptr<void(Arg0,Args...)>
coroutine<std::function<void(Arg0,Args...)>>::
create(std::function<void(Arg0,Args...)> func, std::size_t stack_size, coro_detail::index_sequence<I...>)
{
	auto self = std::shared_ptr<this_type>(new this_type());
	self->m_coro = base_type::coro_context::create(std::function<int(void*)>([self, func](void *data)
	{
		if( data == nullptr )
			throw exception("coroutine<...>: No arguments were passed for the first invoke.");

		auto t = static_cast<std::tuple<Arg0,Args...>*>(data);
		func(std::get<I>(*t)...);

		self->_finished();
		return 0;
	}),
	stack_size);
	return self;
}

/*--------------------------------------------------------------------------------------------------------------------*/

inline coroutine_ptr<void()>
coroutine<std::function<void()>>::
create(std::function<void()> func, std::size_t stack_size)
{
	auto self = std::shared_ptr<this_type>(new this_type());
	auto func_mw = make_move_wrapper(std::move(func));

	self->m_coro = base_type::coro_context::create(std::function<int(void*)>([self, func_mw](void*)
	{
		(*func_mw)();
		self->_finished();
		return 0;
	}),
	stack_size);
	return self;
}

/*--------------------------------------------------------------------------------------------------------------------*/

namespace coro_detail
{

template <typename Func, std::size_t...I>
coro_detail::coroutine_ptr<Func, I...>
create_coroutine(Func &&func, std::size_t stack_size, coro_detail::index_sequence<I...>)
{
	using FT = coro_detail::function_traits<Func>;
	using return_type = typename FT::return_type;
	using arg_types = typename FT::arg_types;

	using coro_t = coroutine<std::function<return_type(typename std::tuple_element<I,arg_types>::type...)>>;
	return coro_t::create(std::forward<Func>(func), stack_size, coro_detail::make_index_sequence<FT::arg_count>{});
}

} //namespace coro_detail

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Func, typename...Args>
auto start_coroutine(Func &&func, Args&&...args, std::size_t stack_size)
->
decltype(create_coroutine(std::forward<Func>(func), stack_size))
{
	auto coro = create_coroutine(std::forward<Func>(func), stack_size);
	coro->invoke(std::forward<Args>(args)...);
	return coro;
}

/*--------------------------------------------------------------------------------------------------------------------*/

inline void coro_yield()
{
	this_coro::yield();
}

template <typename Func, typename...Args>
void coro_yield(Func &&func, Args&&...args)
{
	static_assert(coro_detail::is_functor<Func>::value, "The template type must be a callable type.");
	this_coro::yield(std::forward<Func>(func), std::forward<Args>(args)...);
}

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Ret, typename...Args>
Ret coro_await(coroutine_ptr<Ret(Args...)> coro, Args...args)
{
	return coro_await(*coro, std::forward<Args>(args)...);
}

template <typename Ret, typename...Args>
Ret coro_await(coroutine<std::function<Ret(Args...)>> &coro, Args...args)
{
	auto context = &this_coro::get();
	if( coro.is_finished() )
		return *coro;

	auto func_mw = make_move_wrapper (
		std::bind (
			static_cast<void(coroutine<std::function<Ret(Args...)>>::*)(Args...)> (
				&coroutine<std::function<Ret(Args...)>>::invoke
			),
			&coro,
			std::forward<Args>(args)...
		)
	);
	context->yield([&]
	{
		if( not coro.is_started() )
			(*func_mw)();
		if( coro.is_finished() )
			context->set_again();
		else
		{
			coro.set_finished_callback([context]{
				context->invoke();
			});
		}
	});
	return *coro;
}

template <typename Func, typename...Args>
enable_if_t <
	coro_detail::is_functor<Func>::value,
	decltype(std::declval<Func>()(std::declval<Args>()...))
>
coro_await(Func &&func, Args&&...args, std::size_t stack_size)
{
	return coro_await(gts::create_coroutine(std::forward<Func>(func), stack_size), std::forward<Args>(args)...);
}

GTS_NAMESPACE_END


#endif //GTS_DETAIL_COROUTINE_H

