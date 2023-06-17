#include "args_parser.h"
#include "algorithm.h"

#include <unordered_set>
#include <iostream>

namespace gts { namespace cmdline
{

typedef args_parser::rule         rule;
typedef args_parser::value        value;
typedef args_parser::description  description;
typedef args_parser::arguments    arguments;

typedef std::unordered_set<rule>  args_cache;

class GTS_DECL_HIDDEN parser_impl
{
public:
	explicit parser_impl(const std::string &help_title);

public:
	void add(args_cache &cache, const rule &r, const description &d);
	void print_help();

public:
	std::string m_help_title;
	args_cache m_group;
	args_cache m_flag;
	description m_version;
	description m_v;
	description m_help;
	description m_help_ex;
	bool m_h = false;
};

parser_impl::parser_impl(const std::string &help_title) :
	m_help_title(help_title)
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

void parser_impl::add(args_cache &cache, const rule &r, const description &d)
{
	for(auto arg : string_split(r, ","))
	{
		arg = trimmed(arg);
		if( check(arg) == false )
			continue;

		else if( arg.size() == 1 )
		{
			if( arg != "-" )
				cache.emplace(arg);
		}
		else if( arg == "--" )
			continue ;
		else
			cache.emplace(arg);
	}
	m_help += "    " + r + " :\n        " + d + "\n\n";
}

void parser_impl::print_help()
{
	std::cout << "\n";
	if( not m_help_title.empty() )
		std::cout << m_help_title << "\n\n";

	std::cout << m_help;
	if( not m_help_ex.empty() )
		std::cout << m_help_ex;

	std::cout << std::flush;
	exit(0);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------------*/

args_parser::args_parser(const std::string &help_title) :
	m_impl(new parser_impl(help_title))
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

args_parser &args_parser::add_group(const rule &r, const description &d)
{
	m_impl->add(m_impl->m_group, r,d);
	return *this;
}

args_parser &args_parser::add_flag(const rule &r, const description &d)
{
	m_impl->add(m_impl->m_flag, r,d);
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
	while( not ends_with(m_impl->m_help_ex, "\n\n") )
		m_impl->m_help_ex += "\n";
	return *this;
}

arguments args_parser::parsing(int argc, const char *argv[], string_list &other)
{
	args_cache cache;
	arguments result;

	for(int i=1; i<argc; i++)
	{
		auto pair = cache.emplace(argv[i]);
		if( pair.second == false )
		{
			std::cerr << "Too many parameters." << std::endl;
			exit(-1);
		}
		auto &arg = *pair.first;

		if( arg == "-" or arg == "--" )
		{
			std::cerr << "Invalid argument." << std::endl;
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
			{
				std::cout << m_impl->m_version << std::endl;
				exit(0);
			}
			else if( not m_impl->m_v.empty() )
			{
				std::cout << m_impl->m_version << std::endl;
				exit(0);
			}
			else
			{
				std::cerr << "Invalid argument." << std::endl;
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
				std::cout << m_impl->m_v << std::endl;
				exit(0);
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
				std::cerr << "Invalid argument." << std::endl;
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
				std::cerr << "Invalid argument." << std::endl;
				exit(-1);
			}
			result.emplace(*it, argv[i+1]);
			i++;
			continue ;
		}

		it = m_impl->m_flag.find(arg);
		if( it != m_impl->m_flag.end() )
		{
			result.emplace(*it, *it);
			continue ;
		}

		if( arg.size() <= 2 )
		{
			other.emplace_back(arg);
			continue;
		}

		auto pos = arg.find("=");
		if( pos != std::string::npos )
		{
			pair = cache.emplace(arg.substr(0,pos));
			auto tmp = *pair.first;
			if( pair.second == false )
			{
				std::cerr << "Too many parameters." << std::endl;
				exit(-1);
			}
			auto it2 = m_impl->m_group.find(tmp);
			if( it2 == m_impl->m_group.end() )
				other.emplace_back(arg);
			else
				result.emplace(*it2, arg.substr(pos+1));
			continue;
		}

		if( arg[0] != '-' )
		{
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
		if( flag == false )
		{
			other.emplace_back(arg);
			continue;
		}

		for(j=1; j<arg.size()-1; j++)
		{
			pair = cache.emplace(std::string("-") + arg[j]);
			auto tmp = *pair.first;
			if( pair.second == false )
			{
				std::cerr << "Too many parameters." << std::endl;
				exit(-1);
			}

			auto it2 = m_impl->m_flag.find(tmp);
			if( it2 == m_impl->m_flag.end() )
			{
				other.emplace_back(arg);
				continue;
			}
			else
			{
				result.emplace(*it2, *it2);
				continue ;
			}
		}

		pair = cache.emplace(std::string("-") + arg[j]);
		auto tmp = *pair.first;
		if( pair.second == false )
		{
			std::cerr << "Too many parameters." << std::endl;
			exit(-1);
		}

		auto it2 = m_impl->m_flag.find(tmp);
		if( it2 != m_impl->m_flag.end() )
		{
			result.emplace(*it2, *it2);
			continue ;
		}

		it2 = m_impl->m_group.find(tmp);
		if( it2 != m_impl->m_group.end() )
		{
			if( i + 1 == argc or
				m_impl->m_group.find(argv[i+1]) != m_impl->m_group.end() or
				m_impl->m_flag.find(argv[i+1]) != m_impl->m_flag.end() )
			{
				std::cerr << "Invalid argument." << std::endl;
				exit(-1);
			}
			result.emplace(*it2, argv[i+1]);
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
		std::cerr << "Invalid argument." << std::endl;
		exit(-1);
	}
	return res;
}

bool operator&(const args_parser::arguments &args_hash, args_parser::rule &key)
{
	return args_hash.find(key) != args_hash.end();
}

bool operator&(const args_parser::rule &key, const args_parser::arguments &args_hash)
{
	return args_hash.find(key) != args_hash.end();
}

}} //namespace gts::cmdline
