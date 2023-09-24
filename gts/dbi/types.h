#ifndef GTS_DBI_TYPES_H
#define GTS_DBI_TYPES_H

#include <gts/dbi/cell.h>
#include <vector>
#include <map>

namespace gts { namespace dbi
{

struct connect_info
{
	std::string driver; //odbc using
	std::string ip;
	uint16_t port;
	std::string user;
	std::string passwd;
};

using binary = std::vector<char>;

class row_vector : public std::vector<cell>
{
public:
	using std::vector<cell>::vector;
	using std::vector<cell>::operator[];

public:
	reference operator[](const std::string &column_name)
	{
		for(auto &cell : *this)
		{
			if( cell.column_name() == column_name )
				return cell;
		}
		throw exception(-1, "row_vector::operator[](std::string): 'Invalid column name'.");
	}
};

using table_data = std::vector<row_vector>;

}} //namespace gts::dbi


#endif //GTS_DBI_TYPES_H
