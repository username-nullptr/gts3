#ifndef GTS_GLOBAL_H
#define GTS_GLOBAL_H

#include <string>
#include <deque>

namespace gts
{

#ifdef _MSC_VER

# define GTS_DECL_EXPORT  __declspec(dllexport)
# define GTS_DECL_IMPORT  __declspec(dllimport)
# define GTS_DECL_HIDDEN

# define __attribute_used__    __declspec(used)
# define __attribute_unused__  __declspec(unused)

#elif defined(__GNUC__)

# define GTS_DECL_EXPORT  __attribute__((visibility("default")))
# define GTS_DECL_IMPORT
# define GTS_DECL_HIDDEN  __attribute__((visibility("hidden")))

# ifndef __attribute_used__
#  define __attribute_used__  __attribute__((used))
# endif //__attribute_used__

# ifndef __attribute_unused__
#  define __attribute_unused__  __attribute__((unused))
# endif //__attribute_used__

#else // other compiler

# define GTS_DECL_EXPORT
# define GTS_DECL_IMPORT
# define GTS_DECL_HIDDEN

# define __attribute_used__
# define __attribute_unused__

#endif //compiler

#ifdef gtscore_EXPORTS
# define GTSCORE_API  GTS_DECL_EXPORT
#else //gtscore_EXPORTS
# define GTSCORE_API  GTS_DECL_IMPORT
#endif //gtscore_EXPORTS

#define _UNUSED(x)  (void)(x)

#if defined(_WIN64) || defined(__x86_64__) || defined(__arm64__) || defined(__aarch64__)
# define _OS_64BIT
typedef uint64_t  address_bits_wide;
#else
# define _OS_32BIT
typedef uint32_t  address_bits_wide;
#endif // 32bit & 64bit

#define DISABLE_COPY(_class) \
	explicit _class(const _class&) = delete; \
	explicit _class(_class&&) = delete; \
	void operator=(const _class&) = delete; \
	void operator=(_class&&) = delete;

typedef std::deque<std::string>  string_list;

} //namespace gts


#endif //GTS_GLOBAL_H
