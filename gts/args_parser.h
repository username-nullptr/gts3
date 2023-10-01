
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

#ifndef GTS_ARGS_PARSER_H
#define GTS_ARGS_PARSER_H

#include <gts/string_list.h>
#include <unordered_map>
#include <functional>

namespace gts { namespace cmdline
{

class parser_impl;

class GTSCORE_API args_parser
{
	GTS_DISABLE_COPY_MOVE(args_parser)

public:
	typedef std::string  rule;
	typedef std::string  value;
	typedef std::string  description;
	typedef std::string  identification;
	typedef std::unordered_map<rule, value>  arguments;

public:
	explicit args_parser(const std::string &help_title = {});
	~args_parser();

public:
	args_parser &set_help_title(const std::string &text);

public:
	// ./a.out -f filename
	// ./a.out --file=filename
	args_parser &add_group(const rule &r, const description &d, const identification &i = {});

	// ./a.out -abc
	// ./a.out -a -b -c
	args_parser &add_flag(const rule &r, const description &d, const identification &i = {});

public:
	args_parser &set_version(const description &d);
	args_parser &set_v(const description &d);

public:
	args_parser &enable_h();
	args_parser &disable_h();
	args_parser &set_help_extension(const description &d);

public:
	arguments parsing(int argc, const char *argv[], string_list &other);
	arguments parsing(int argc, const char *argv[]); // exit if other.

private:
	parser_impl *m_impl;
};

}} //namespace gts::cmdline

GTSCORE_API bool operator&(const gts::cmdline::args_parser::arguments &args_hash, const gts::cmdline::args_parser::rule &key);
GTSCORE_API bool operator&(const gts::cmdline::args_parser::rule &key, const gts::cmdline::args_parser::arguments &args_hash);


#endif //GTS_ARGS_PARSER_H
