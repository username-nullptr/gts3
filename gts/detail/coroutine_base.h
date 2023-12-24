#ifndef GTS_DETAIL_COROUTINE_BASE_H
#define GTS_DETAIL_COROUTINE_BASE_H

#include <libcopp/coroutine/coroutine_context_container.h>
#include <gts/detail/coro_utilities.h>
#include <gts/exception.h>

GTS_NAMESPACE_BEGIN

template <typename T>
class coroutine;

template <typename T>
using coroutine_ptr = std::shared_ptr<coroutine<T>>;

namespace coro_detail
{

class coroutine_base
{
	GTS_DISABLE_COPY(coroutine_base)

protected:
	using coro_context = copp::coroutine_context_default;
	using coro_context_ptr = copp::coroutine_context_default::ptr_t;

public:
	coroutine_base() = default;
	virtual ~coroutine_base() = 0;

public:
	void set_finished_callback(std::function<void()> func);
	void set_again(bool flag = true);

public:
	bool is_yield() const noexcept;
	bool is_finished() const noexcept;

public:
	void invoke() noexcept(false);
	void yield() noexcept(false);

	template <typename Func, typename...Args>
	void yield(Func &&func, Args&&...args)
	{
		static_assert(is_functor<Func>::value, "The template type must be a callable type.");
		m_yield_func = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
		yield();
	}

protected:
	void _invoke() noexcept(false);
	void _finished();

protected:
	coroutine_base(coroutine_base&&) noexcept = default;
	coroutine_base &operator=(coroutine_base&&) noexcept = default;

protected:
	coro_context_ptr m_coro;
	std::function<void()> m_yield_func;
	std::function<void()> m_finished_callback;
	std::atomic_bool m_yield {false};
	bool m_again = false;
};

/*--------------------------------------------------------------------------------------------------------------------*/

using coro_context = coro_detail::coroutine_base;

class this_coro
{
	GTS_DISABLE_COPY_MOVE(this_coro)

public:
	virtual ~this_coro() = 0;

public:
	static bool is_valid() noexcept;
	static coro_context &get() noexcept(false);

public:
	static void invoke();
	static void yield();

	template <typename Func, typename...Args>
	static void yield(Func &&func, Args...args)
	{
		static_assert(coro_detail::is_functor<Func>::value, "The template type must be a callable type.");
		get().yield(std::forward<Func>(func), std::forward<Args>(args)...);
	}

protected:
	static void _set(coroutine_base*);

private:
	friend class coroutine_base;

	template <typename Arg0, typename...Args>
	friend class coroutine_base_args;
};

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Arg0, typename...Args>
class coroutine_base_args : public coroutine_base
{
	GTS_DISABLE_COPY(coroutine_base_args)

public:
	coroutine_base_args() = default;
	virtual ~coroutine_base_args() = 0;

public:
	using coroutine_base::invoke;
	void invoke(const Arg0 arg0, const Args...args) noexcept(false)
	{
		if( not this_coro::is_valid() )
			return _invoke(std::forward<Arg0>(arg0), std::forward<Args>(args)...);

		auto &context = this_coro::get();
		if( &context == this )
			return ;

		context.yield([&]
		{
			context.m_again = true;
			_invoke(std::forward<Arg0>(arg0), std::forward<Args>(args)...);
		});
	}

protected:
	coroutine_base_args(coroutine_base_args&&) noexcept = default;
	coroutine_base_args &operator=(coroutine_base_args&&) noexcept = default;

protected:
	using coroutine_base::_invoke;
	void _invoke(const Arg0 arg0, const Args...args) noexcept(false)
	{
		m_again = false;
		this_coro::_set(this);
		auto t = std::tuple<Arg0,Args...>(arg0,args...);

		auto _r = m_coro->start(&t);
		if( _r != 0 )
			throw exception("coroutine_base_args<...>::invoke: coroutine invoke failed ({}).", _r);

		else if( m_yield_func )
		{
			m_yield_func();
			m_yield_func = {};
		}
		if( m_again )
			_invoke();
	}
};

template <typename Arg0, typename...Args>
inline coroutine_base_args<Arg0,Args...>::~coroutine_base_args() = default;

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Ret>
class coroutine_base_return
{
	GTS_DISABLE_COPY(coroutine_base_return)

public:
	coroutine_base_return() = default;
	virtual ~coroutine_base_return() = 0;

public:
	Ret result() noexcept(false)
	{
		if( m_ret == nullptr )
			throw exception("coroutine_base_return<T>::result: coroutine is not over yet.");
		return *m_ret;
	}

public:
	inline Ret operator*() noexcept(false) {
		return result();
	}

	inline Ret *operator->() noexcept(false)
	{
		if( m_ret == nullptr )
			throw exception("coroutine_base_return<T>::operator->: coroutine is not over yet.");
		return m_ret;
	}

protected:
	void save_return(Ret ret) {
		assignment_helper<Ret>::call(&m_ret, m_ret_s, ret);
	}

protected:
	template <typename T>
	struct assignment_helper
	{
		static void call(decay_t<Ret> **ret, decay_t<Ret> &ret_s, T t)
		{
			ret_s = std::move(t);
			*ret = &ret_s;
		}
	};

protected:
	template <typename T>
	struct assignment_helper<T&>
	{
		static void call(decay_t<Ret> **ret, decay_t<Ret>&, T &t) {
			*ret = &t;
		}
	};

protected:
	coroutine_base_return(coroutine_base_return&&) noexcept = default;
	coroutine_base_return &operator=(coroutine_base_return&&) noexcept = default;

protected:
	decay_t<Ret> *m_ret;
	decay_t<Ret> m_ret_s;
};

template <typename Ret>
inline coroutine_base_return<Ret>::~coroutine_base_return() = default;

/*--------------------------------------------------------------------------------------------------------------------*/

template <>
class coroutine_base_return<void>
{
	GTS_DISABLE_COPY(coroutine_base_return)

public:
	coroutine_base_return() = default;
	virtual ~coroutine_base_return() = 0;

public:
	void result() noexcept {}
	void operator*() noexcept {}

protected:
	coroutine_base_return(coroutine_base_return&&) noexcept = default;
	coroutine_base_return &operator=(coroutine_base_return&&) noexcept = default;
};

inline coroutine_base_return<void>::~coroutine_base_return() = default;

/*--------------------------------------------------------------------------------------------------------------------*/

template <typename Func, std::size_t...I>
using coroutine_ptr = gts::coroutine_ptr <
	std::function <
		typename coro_detail::function_traits<Func>::return_type (
			tuple_element_t< I, typename coro_detail::function_traits<Func>::arg_types >...
		)
	>
>;

template <typename Func, std::size_t...I>
coro_detail::coroutine_ptr<Func, I...>
create_coroutine(Func &&func, std::size_t stack_size, coro_detail::index_sequence<I...>);

} //namespace coro_detail

GTS_NAMESPACE_END


#endif //GTS_DETAIL_COROUTINE_BASE_H

