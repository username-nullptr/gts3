
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

#include "args_parser.h"
#include "algorithm.h"

#include <unordered_set>
#include <iostream>

GTS_CMDLINE_NAMESPACE_BEGIN

typedef args_parser::rule            rule;
typedef args_parser::description     description;
typedef args_parser::arguments       arguments;
typedef args_parser::identification  identification;

typedef std::string  group_iden;
typedef std::unordered_set<group_iden>  parsing_cache;

struct arg_info
{
	group_iden group;
	identification iden;
};
typedef std::unordered_map<rule, arg_info>  args_cache;

class GTS_DECL_HIDDEN args_parser_impl
{
public:
	explicit args_parser_impl(std::string &help_title);
	void add(args_cache &cache, const rule &r, const description &d, const identification &i);

public:
	[[noreturn]] void print_version() const;
	[[noreturn]] void print_v() const;
	[[noreturn]] void print_help() const;

public:
	args_cache m_group;
	args_cache m_flag;

	description m_version;
	description m_v;

	std::string m_help_title;
	description m_help;
	description m_help_ex;

	bool m_h = false;
};

args_parser_impl::args_parser_impl(std::string &help_title) :
	m_help_title(std::move(help_title))
{

}

static bool check(const std::string &str)
{
	if( str.empty() )
		return false;

	for(std::size_t i=1; i<str.size()-1; i++)
	{
		if( str[i] < 33 or str[i] == 61 or str[i] == 92 or str[i] > 126 )
			return false;
	}
	return true;
}

void args_parser_impl::add(args_cache &cache, const rule &r, const description &d, const identification &i)
{
	static std::size_t id_source = 0;
	char buf[128] = "";
	sprintf(buf, "%zu", id_source++);

	for(auto arg : str_split(r, ","))
	{
		arg = str_trimmed(arg);
		if( not check(arg) or arg == "--" )
			continue;

		else if( arg.size() == 1 )
		{
			if( arg != "-" )
				cache.emplace(arg, arg_info{buf, i.empty()? arg : i});
		}
		else
			cache.emplace(arg, arg_info{buf, i.empty()? arg : i});
	}
	m_help += "    " + r + " :\n        " + d + "\n\n";
}

[[noreturn]] void args_parser_impl::print_version() const
{
	std::cout << "\n" << m_version << "\n\n" << std::flush;
	exit(0);
}

[[noreturn]] void args_parser_impl::print_v() const
{
	std::cout << "\n" << m_v << "\n\n" << std::flush;
	exit(0);
}

[[noreturn]] void args_parser_impl::print_help() const
{
	std::cout << "\n";
	if( not m_help_title.empty() )
		std::cout << m_help_title << "\n\n";

	std::cout << m_help;

	if( m_v.empty() )
		std::cout << "    --version:";
	else
		std::cout << "    -v, --version:";
	std::cout << "\n        Viewing server version.\n\n";

	if( m_h )
		std::cout << "    -h, --help:";
	else
		std::cout << "    --help:";
	std::cout << "\n        Viewing help.\n\n";

	if( not m_help_ex.empty() )
		std::cout << m_help_ex << "\n\n";

	std::cout << std::flush;
	exit(0);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------------*/

args_parser::args_parser(std::string help_title) :
	m_impl(new args_parser_impl(help_title))
{

}

args_parser::~args_parser()
{
	delete m_impl;
}

args_parser &args_parser::set_help_title(const std::string &text)
{
	m_impl->m_help_title = text;
	return *this;
}

args_parser &args_parser::add_group(const rule &r, const description &d, const identification &i)
{
	m_impl->add(m_impl->m_group, r,d,i);
	return *this;
}

args_parser &args_parser::add_flag(const rule &r, const description &d, const identification &i)
{
	m_impl->add(m_impl->m_flag, r,d,i);
	return *this;
}

args_parser &args_parser::set_version(const description &d)
{
	m_impl->m_version = d;
	return *this;
}

args_parser &args_parser::set_v(const description &d)
{
	m_impl->m_v = d;
	return *this;
}

args_parser &args_parser::enable_h()
{
	m_impl->m_h = true;
	return *this;
}

args_parser &args_parser::disable_h()
{
	m_impl->m_h = false;
	return *this;
}

args_parser &args_parser::set_help_extension(const description &d)
{
	m_impl->m_help_ex = d;
	return *this;
}

static void group_check_duplication(parsing_cache &cache, const group_iden &group)
{
	if( not cache.emplace(group).second )
	{
		std::cerr << "Too many parameters." << std::endl;
		exit(-1);
	}
}

arguments args_parser::parsing(int argc, const char *argv[], string_list &other)
{
	arguments result;
	parsing_cache cache;

	for(int i=1; i<argc; i++)
	{
		std::string arg(argv[i]);
		if( arg == "-" or arg == "--" )
		{
			std::cerr << "Invalid arguments." << std::endl;
			exit(-1);
		}
		else if( arg == "--version" )
		{
			if( argc > 2 )
			{
				std::cerr << "Too many parameters." << std::endl;
				exit(-1);
			}
			else if( not m_impl->m_version.empty() )
				m_impl->print_version();
			else if( not m_impl->m_v.empty() )
				m_impl->print_v();
			else
			{
				std::cerr << "Invalid arguments." << std::endl;
				exit(-1);
			}
		}
		else if( arg == "-v" )
		{
			if( not m_impl->m_v.empty() )
			{
				if( argc > 2 )
				{
					std::cerr << "Too many parameters." << std::endl;
					exit(-1);
				}
				m_impl->print_v();
			}
		}
		else if( arg == "--help" )
		{
			if( argc > 2 )
			{
				std::cerr << "Too many parameters." << std::endl;
				exit(-1);
			}
			else if( not m_impl->m_help.empty() )
				m_impl->print_help();
			else
			{
				std::cerr << "Invalid arguments." << std::endl;
				exit(-1);
			}
		}
		else if( arg == "-h" )
		{
			if( m_impl->m_h )
			{
				if( argc > 2 )
				{
					std::cerr << "Too many parameters." << std::endl;
					exit(-1);
				}
				m_impl->print_help();
			}
		}
		auto it = m_impl->m_group.find(arg);
		if( it != m_impl->m_group.end() )
		{
			if( i + 1 == argc or
				m_impl->m_group.find(argv[i+1]) != m_impl->m_group.end() or
				m_impl->m_flag.find(argv[i+1]) != m_impl->m_flag.end() )
			{
				std::cerr << "Invalid arguments." << std::endl;
				exit(-1);
			}
			group_check_duplication(cache, it->second.group);
			result.emplace(it->second.iden, argv[i+1]);
			i++;
			continue ;
		}
		it = m_impl->m_flag.find(arg);
		if( it != m_impl->m_flag.end() )
		{
			group_check_duplication(cache, it->second.group);
			result.emplace(it->second.iden, it->second.iden);
			continue ;
		}
		else if( arg.size() <= 2 )
		{
			group_check_duplication(cache, arg);
			other.emplace_back(arg);
			continue;
		}
		auto pos = arg.find('=');
		if( pos != std::string::npos )
		{
			auto it2 = m_impl->m_group.find(arg.substr(0,pos));
			if( it2 == m_impl->m_group.end() )
			{
				group_check_duplication(cache, arg);
				other.emplace_back(arg);
			}
			else
			{
				group_check_duplication(cache, it2->second.group);
				result.emplace(it2->second.iden, arg.substr(pos+1));
			}
			continue;
		}
		if( arg[0] != '-' )
		{
			group_check_duplication(cache, arg);
			other.emplace_back(arg);
			continue;
		}
		bool flag = true;
		std::size_t j = 1;

		for(; j<arg.size(); j++)
		{
			if( arg[j] < 'A' or (arg[j] > 'Z' and arg[j] < 'a') or arg[j] > 'z' )
			{
				flag = false;
				break;
			}
		}
		if( not flag )
		{
			group_check_duplication(cache, arg);
			other.emplace_back(arg);
			continue;
		}
		for(j=1; j<arg.size()-1; j++)
		{
			auto it2 = m_impl->m_flag.find(std::string("-") + arg[j]);
			if( it2 == m_impl->m_flag.end() )
			{
				group_check_duplication(cache, arg);
				other.emplace_back(arg);
				continue;
			}
			else
			{
				group_check_duplication(cache, it2->second.group);
				result.emplace(it2->second.iden, it2->second.iden);
				continue ;
			}
		}
		auto tmp = std::string("-") + arg[j];

		auto it2 = m_impl->m_flag.find(tmp);
		if( it2 != m_impl->m_flag.end() )
		{
			group_check_duplication(cache, it2->second.group);
			result.emplace(it2->second.iden, it2->second.iden);
			continue ;
		}
		it2 = m_impl->m_group.find(tmp);
		if( it2 != m_impl->m_group.end() )
		{
			if( i + 1 == argc or
				m_impl->m_group.find(argv[i+1]) != m_impl->m_group.end() or
				m_impl->m_flag.find(argv[i+1]) != m_impl->m_flag.end() )
			{
				std::cerr << "Invalid arguments." << std::endl;
				exit(-1);
			}
			group_check_duplication(cache, it2->second.group);
			result.emplace(it2->second.iden, argv[i+1]);
			i++;
			continue ;
		}
		other.emplace_back(arg);
	}
	return result;
}

arguments args_parser::parsing(int argc, const char *argv[])
{
	string_list other;
	auto res = parsing(argc, argv, other);
	if( not other.empty() )
	{
		std::cerr << "Invalid arguments." << std::endl;
		exit(-1);
	}
	return res;
}

GTS_CMDLINE_NAMESPACE_END

using namespace gts::cmdline;

bool operator&(const args_parser::arguments &args_hash, const args_parser::rule &key)
{
	return args_hash.find(key) != args_hash.end();
}

bool operator&(const args_parser::rule &key, const args_parser::arguments &args_hash)
{
	return args_hash.find(key) != args_hash.end();
}
