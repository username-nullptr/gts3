
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef GTS_GLOBAL_H
#define GTS_GLOBAL_H

#include <gts/gts_enable_ssl.h>
#include <nlohmann/json.hpp>
#include <gts/utility.h>
#include <asio.hpp>
#include <vector>

namespace gts
{

#ifdef _MSC_VER

# define GTS_DECL_EXPORT  __declspec(dllexport)
# define GTS_DECL_IMPORT  __declspec(dllimport)
# define GTS_DECL_HIDDEN

# define __attribute_used__    __declspec(used)
# define __attribute_unused__  __declspec(unused)

# define __attribute_weak__              __declspec(weak)
# define __attribute_weakref__(_symbol)  __declspec(weakref(_symbol))

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

# define __attribute_weak__              __attribute__((weak))
# define __attribute_weakref__(_symbol)  __attribute__((weakref(_symbol)))

#else // other compiler

# define GTS_DECL_EXPORT
# define GTS_DECL_IMPORT
# define GTS_DECL_HIDDEN

# define __attribute_used__
# define __attribute_unused__

# define __attribute_weak__
# define __attribute_weakref__(_symbol)

#endif //compiler

#define C_VIRTUAL_FUNC             __attribute_weak__
#define C_VIRTUAL_SYMBOL(_symbol)  __attribute_weakref__(_symbol)

#ifdef gtscore_EXPORTS
# define GTSCORE_API  GTS_DECL_EXPORT
#else //gtscore_EXPORTS
# define GTSCORE_API  GTS_DECL_IMPORT
#endif //gtscore_EXPORTS

GTSCORE_API std::string version_string();

GTSCORE_API asio::io_context &io_context();

template <typename T> inline
void delete_later(T *obj)
{
	io_context().post([obj]{
		delete obj;
	});
}

struct site_info
{
	std::string addr;
	uint16_t port;
	bool universal;

#ifdef GTS_ENABLE_SSL
	bool ssl;
#endif //ssl

	nlohmann::json json;
};
typedef std::map<std::string, site_info>  site_info_map;

GTSCORE_API site_info_map get_site_infos();

} //namespace gts


#endif //GTS_GLOBAL_H
