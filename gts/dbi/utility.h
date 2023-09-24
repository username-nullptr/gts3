#ifndef DBI_UTILITY_H
#define DBI_UTILITY_H

#include <utility>

namespace dbi
{

template <bool C, typename T = void>
using enable_if_t = typename std::enable_if<C,T>::type;

template <typename T>
using decay_t = typename std::decay<T>::type;

#define dbi_is_same(...)   std::is_same<__VA_ARGS__>::value

#define dbi_is_enum(...)   std::is_enum<__VA_ARGS__>::value

#define dbi_is_base_of(...)   std::is_base_of<__VA_ARGS__>::value

} //namesapce dbi

#define DBI_DECLVAL(...)   std::declval<__VA_ARGS__>()

#define DBI_CLASS_METHOD_DECLVAL(Class, Return, ...)   (DBI_DECLVAL(Class).*DBI_DECLVAL(Return(Class::*)(__VA_ARGS__)))

#define DBI_TYPE_DECLTYPE(...)   typename _U_0 = decltype(__VA_ARGS__)

#define DBI_TYPE_ENABLE_IF(...)   typename _U_0 = dbi::enable_if_t<__VA_ARGS__>


#endif //DBI_UTILITY_H
