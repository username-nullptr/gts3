
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

#if GTS_CPLUSPLUS >= 201703L
# define GTS_APP_NAMESPACE_BEGIN  namespace gts::app {
# define GTS_APP_NAMESPACE_END    } //namespace gts::app
# define GTS_CMDLINE_NAMESPACE_BEGIN  namespace gts::cmdline {
# define GTS_CMDLINE_NAMESPACE_END    } //namespace gts::cmdline
# define GTS_EXTENSION_NAMESPACE_BEGIN  namespace gts::extension {
# define GTS_EXTENSION_NAMESPACE_END    } //namespace gts::extension
#else //cpp17
# define GTS_APP_NAMESPACE_BEGIN  namespace gts { namespace app {
# define GTS_APP_NAMESPACE_END    }} //namespace gts::app
# define GTS_CMDLINE_NAMESPACE_BEGIN  namespace gts { namespace cmdline {
# define GTS_CMDLINE_NAMESPACE_END    }} //namespace gts::cmdline
# define GTS_EXTENSION_NAMESPACE_BEGIN  namespace gts { namespace extension {
# define GTS_EXTENSION_NAMESPACE_END    }} //namespace gts::extension
#endif //cpp17

GTS_NAMESPACE_BEGIN

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

# define GTS_CXX_ATTR_USED    __attribute__((used))
# define GTS_CXX_ATTR_UNUSED  __attribute__((unused))

# define GTS_CXX_ATTR_WEAK              __attribute__((weak))
# define GTS_CXX_ATTR_WEAKREF(_symbol)  __attribute__((weakref(_symbol)))

#else // other compiler

# define GTS_DECL_EXPORT
# define GTS_DECL_IMPORT
# define GTS_DECL_HIDDEN

# define GTS_CXX_ATTR_USED
# define GTS_CXX_ATTR_UNUSED

# define GTS_CXX_ATTR_WEAK
# define GTS_CXX_ATTR_WEAKREF(_symbol)

#endif //compiler

#define C_VIRTUAL_FUNC             __attribute_weak__
#define C_VIRTUAL_SYMBOL(_symbol)  __attribute_weakref__(_symbol)

#ifdef gtscore_EXPORTS
# define GTSCORE_API  GTS_DECL_EXPORT
#else //gtscore_EXPORTS
# define GTSCORE_API  GTS_DECL_IMPORT
#endif //gtscore_EXPORTS

GTSCORE_API GTS_CXX_NODISCARD("") std::string version_string();

GTSCORE_API GTS_CXX_NODISCARD("") asio::io_context &io_context();

template <typename T> inline
void delete_later(T *obj, asio::io_context &io = io_context())
{
	io.post([obj]{
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

GTSCORE_API GTS_CXX_NODISCARD("") site_info_map get_site_infos();

GTS_NAMESPACE_END


#endif //GTS_GLOBAL_H
