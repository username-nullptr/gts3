
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

#ifndef GTS_GTS_NAMESPACE_H
#define GTS_GTS_NAMESPACE_H

#include <gts/cplusplus.hpp>

#define GTS_NAMESPACE_BEGIN  namespace gts {
#define GTS_NAMESPACE_END    } //namespace gts

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


#endif //GTS_GTS_NAMESPACE_H
