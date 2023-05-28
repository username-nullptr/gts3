#include "json.h"
#include <cppfilesystem>
#include <fmt/format.h>

namespace rapidjson
{

std::string from_file(const std::string &file_name, rapidjson::Document &json_object)
{
	if( not fs::exists(file_name) )
		return fmt::format("No such file ({})", file_name);

	std::fstream file(file_name);
	if( not file.is_open() )
		return fmt::format("Unable to open json file ({})", file_name);

	auto file_size = fs::file_size(file_name);
	char *buf = new char[file_size + 1] {0};
	file_size = file.readsome(buf, file_size);

	bool has_error = json_object.Parse(buf).HasParseError();
	delete[] buf;

	if( has_error )
		return fmt::format("Parsing failed ({})", file_name);
	return "";
}

} //namespace rapidjson
