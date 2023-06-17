#ifndef GTS_ARGS_PARSING_H
#define GTS_ARGS_PARSING_H

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

GTSCORE_API bool operator&(const args_parser::arguments &args_hash, args_parser::rule &key);
GTSCORE_API bool operator&(const args_parser::rule &key, const args_parser::arguments &args_hash);

}} //namespace gts::cmdline


#endif //GTS_ARGS_PARSING_H
