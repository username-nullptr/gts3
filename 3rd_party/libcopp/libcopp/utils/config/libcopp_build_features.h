# pragma once

#include "libcopp/utils/config/compile_optimize.h"

#define LIBCOPP_COPP_NAMESPACE_BEGIN namespace copp { inline namespace v2002 {
#define LIBCOPP_COPP_NAMESPACE_END } }
#define LIBCOPP_COPP_NAMESPACE_ID copp::v2002

#ifdef libcopp_EXPORTS
# define LIBCOPP_COPP_API  UTIL_SYMBOL_EXPORT
#else
# define LIBCOPP_COPP_API  UTIL_SYMBOL_IMPORT
#endif


#define LIBCOPP_COPP_API_HEAD_ONLY UTIL_SYMBOL_VISIBLE
#define LIBCOPP_COPP_API_C(R) extern "C" LIBCOPP_COPP_API R __cdecl

#ifdef __unix__
# define LIBCOPP_MACRO_SYS_POSIX 1
#else
# define LIBCOPP_MACRO_SYS_WIN 1
#endif

#ifndef LIBCOPP_FCONTEXT_USE_TSX
# define LIBCOPP_FCONTEXT_USE_TSX 1
#endif

#ifdef __unix__
# define THREAD_TLS_USE_PTHREAD 1
#endif

#if defined(__cpp_exceptions)
# define LIBCOPP_MACRO_HAS_EXCEPTION __cpp_exceptions
#elif defined(__EXCEPTIONS) && __EXCEPTIONS
# define LIBCOPP_MACRO_HAS_EXCEPTION 1
#elif defined(__clang__)
# if __has_feature(cxx_exceptions)
#  define LIBCOPP_MACRO_HAS_EXCEPTION 1
# endif
#elif defined(_MSC_VER)
# if defined(_CPPUNWIND)
#  define LIBCOPP_MACRO_HAS_EXCEPTION 1
# endif
#endif

#define LIBCOPP_MACRO_ENABLE_EXCEPTION 1
#define LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR 1
#define LIBCOPP_MACRO_ENABLE_RTTI 1

// Patch for disable RTTI on later options
#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
#  if defined(_MSC_VER)
#    if !defined(_CPPRTTI)
#      undef LIBCOPP_MACRO_ENABLE_RTTI
#    endif
#  elif defined(__GNUC__) && __GNUC__ >= 4
#    if !defined(__GXX_RTTI)
#      undef LIBCOPP_MACRO_ENABLE_RTTI
#    endif
#  endif
#endif

#define LIBCOPP_MACRO_NOEXCEPT noexcept
