#include "algorithm.h"
#include <algorithm>

namespace gts
{

string_list string_split(const std::string &str, const std::string &splits, bool ignore_empty)
{
	string_list result;
	if( str.empty() )
		return result;

	auto strs = str + splits;
	auto pos = strs.find(splits);
	int step = splits.size();

	while( pos != std::string::npos )
	{
		auto tmp = strs.substr(0, pos);
		if( not tmp.empty() or not ignore_empty )
			result.emplace_back(std::move(tmp));

		strs = strs.substr(pos + step, strs.size());
		pos = strs.find(splits);
	}
	return result;
}

std::string string_list_join(const string_list &list, const std::string &splits)
{
	std::string result;
	if( list.empty() )
		return result;

	std::size_t i = 0;
	for(; i<list.size()-1; i++)
		result += list[i] + splits;

	result += list[i];
	return result;
}

#if __cplusplus > 201703L

bool starts_with(const std::string &str, const std::string &prefix)
{
	return str.starts_with(prefix);
}

bool ends_with(const std::string &str, const std::string &suffix)
{
	return str.ends_with(suffix);
}

#else //<c++20

bool starts_with(const std::string &str, const std::string &prefix)
{
	if( str.size() < prefix.size() )
		return false;

	for(std::size_t i=0; i<prefix.size(); i++)
	{
		if( str[i] != prefix[i] )
			return false;
	}
	return true;
}

bool ends_with(const std::string &str, const std::string &suffix)
{
	if( str.size() < suffix.size() )
		return false;

	for(std::size_t i=suffix.size(); i>0; i--)
	{
		if( str[str.size()-i] != suffix[i-1] )
			return false;
	}
	return true;
}

#endif //c++20

std::string to_lower(const std::string &str)
{
	auto tmp = str;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
	return tmp;
}

std::string to_upper(const std::string &str)
{
	auto tmp = str;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
	return tmp;
}

std::size_t replace(std::string &str, const std::string &_old, const std::string &_new)
{
	std::size_t sum = 0;
	std::size_t old_pos = 0;
	while( str.find(_old, old_pos) != std::string::npos )
	{
		int start = str.find(_old, old_pos);
		str.replace(start, _old.size(), _new);
		old_pos = start + _new.size();
	}
	return sum;
}

std::string trimmed(const std::string &str)
{
	std::string result;

	std::size_t left = 0;
	while( left < str.size() )
	{
		if( str[left] >= 1 and str[left] <= 32 )
			left++;
		else
			break;
	}
	if( left >= str.size() )
		return result;

	int right = static_cast<int>(str.size() - 1);
	while( right >= 0 )
	{
		if( str[right] >= 1 and str[right] <= 32 )
			right--;
		else
			break;
	}
	if( right < 0 )
		return result;

	result = str.substr(0, right + 1);
	result = result.substr(left);
	return result;
}

std::string from_percent_encoding(const std::string &str)
{
	std::string result;
	if( str.empty() )
		return result;

	result = str;
	char *data = const_cast<char*>(result.c_str());
	const char *inputPtr = result.c_str();

	std::size_t i = 0;
	std::size_t len = str.size();
	std::size_t outlen = 0;
	int a, b;
	char c;

	while( i < len )
	{
		c = inputPtr[i];
		if( c == '%' and i + 2 < len )
		{
			a = inputPtr[++i];
			b = inputPtr[++i];

			if( a >= '0' and a <= '9' )
				a -= '0';

			else if( a >= 'a' and a <= 'f' )
				a = a - 'a' + 10;

			else if( a >= 'A' && a <= 'F' )
				a = a - 'A' + 10;

			if( b >= '0' and b <= '9' )
				b -= '0';

			else if( b >= 'a' and b <= 'f' )
				b  = b - 'a' + 10;

			else if( b >= 'A' and b <= 'F' )
				b  = b - 'A' + 10;

			*data++ = static_cast<char>((a << 4) | b);
		}
		else
			*data++ = c;

		++i;
		++outlen;
	}

	if( outlen != len )
		result = result.substr(0, outlen);
	return result;
}

std::string file_name(const std::string &file_name)
{
	auto pos = file_name.rfind("/");
	if( pos == std::string::npos )
		return file_name;
	return file_name.substr(pos + 1);
}

std::string file_path(const std::string &file_name)
{
	auto pos = file_name.rfind("/");
	if( pos == std::string::npos )
		return ".";
	return file_name.substr(0, pos + 1);
}

} //namespace gts
