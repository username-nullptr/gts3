#ifndef GTS_UTILITY_H
#define GTS_UTILITY_H

#include <rttr/variant.h>

#define GTS_UNUSED(x)  (void)(x)

#if defined(_WIN64) || defined(__x86_64__) || defined(__arm64__) || defined(__aarch64__)
# define GTS_OS_64BIT
#else
# define _OS_32BIT
#endif // 32bit & 64bit

#define GTS_DISABLE_COPY(_class) \
	explicit _class(const _class&) = delete; \
	void operator=(const _class&) = delete;

#define GTS_DISABLE_MOVE(_class) \
	explicit _class(_class&&) = delete; \
	void operator=(_class&&) = delete;

#define GTS_DISABLE_COPY_MOVE(_class) \
	GTS_DISABLE_COPY(_class) GTS_DISABLE_MOVE(_class)

namespace gts
{

template <bool C, typename T = void>
using enable_if_t = typename std::enable_if<C,T>::type;

template <typename T>
using decay_t = typename std::decay<T>::type;

#define gts_is_same(...)   std::is_same<__VA_ARGS__>::value

#define gts_is_base_of(...)   std::is_base_of<__VA_ARGS__>::value

} //namesapce gts

#define GTS_DECLVAL(...)   std::declval<__VA_ARGS__>()

#define GTS_CLASS_METHOD_DECLVAL(Class, Return, ...)   (GTS_DECLVAL(Class).*GTS_DECLVAL(Return(Class::*)(__VA_ARGS__)))

#define GTS_TYPE_DECLTYPE(...)   typename T = decltype(__VA_ARGS__)

#define GTS_TYPE_ENABLE_IF(...)   typename T = gts::enable_if_t<__VA_ARGS__>

#define GTS_RTTR_TYPE(...)   rttr::type::get<__VA_ARGS__>()

#define GTS_RTTR_TYPE_BY_NAME(...)   rttr::type::get_by_name(__VA_ARGS__)


#endif //GTS_UTILITY_H
