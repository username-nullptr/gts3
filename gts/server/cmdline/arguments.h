
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

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "gts/args_parser.h"

namespace gts { namespace cmdline
{

#define GC_SA_START    "start"
#define GC_SA_CFPATH   "-f"
#define GC_SA_DAEMON   "-d"
#define GC_SA_INSNAME  "-i"
#define GC_SA_STOP     "stop"
#define GC_SA_RESTART  "restart"
#define GC_SA_STATUS   "status"
#define GC_SA_VASUS    "--view-subserver-all"
#define GC_SA_VRSUS    "--view-subserver"
#define GC_SA_STSSA    "--start-subserver-all"
#define GC_SA_SPSSA    "--stop-subserver-all"
#define GC_SA_STSS     "--start-subserver"
#define GC_SA_SPSS     "--stop-subserver"

args_parser::arguments argument_check(int argc, const char *argv[], string_list &others);

}} //namespace gts::cmdline


#endif //ARGUMENTS_H
