#ifndef GTS_DETAIL_CORO_UTILITIES_H
#define GTS_DETAIL_CORO_UTILITIES_H

#include <gts/detail/function_traits.h>

GTS_CORO_DETAIL_NAMESPACE_BEGIN

template <std::size_t...I>
struct index_sequence {};

template <std::size_t N, std::size_t...M>
struct make_index_sequence : public make_index_sequence<N-1, N-1, M...> {};

template <std::size_t...M>
struct make_index_sequence<0,M...> : public index_sequence<M...> {};

template <typename T>
T &remove_const(const T &t) {
	return const_cast<T&>(t);
}

template <typename Tup0, typename Tup1, std::size_t...I>
void tuple_assignment(const Tup0 &t0, Tup1 &t1, index_sequence<I...>)
{
	int array[] = { remove_const((std::get<I>(t0)) = *std::get<I>(t1))..., 0 };
	(void)(array);
}

GTS_CORO_DETAIL_NAMESPACE_END


#endif //GTS_DETAIL_CORO_UTILITIES_H
