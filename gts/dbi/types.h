#ifndef DBI_TYPES_H
#define DBI_TYPES_H

#include <dbi/cell_data.h>
#include <vector>
#include <map>

namespace dbi
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

class row_vector : public std::vector<cell_data>
{
public:
	using std::vector<cell_data>::vector;
	using std::vector<cell_data>::operator[];

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

} //namespace dbi


#endif //DBI_TYPES_H
