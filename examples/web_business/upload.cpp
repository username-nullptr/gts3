#include <cppfilesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

int main()
{
	const char *file_name = std::getenv("HTTP_UPLOAD_FILE_NAME");
	if( file_name == nullptr )
	{
		std::cout << "HTTP/1.1 421 Internal Server Error\r\n"
					 "Content-length: 31\r\n"
					 "\r\n"
					 "421 HTTP_UPLOAD_FILE_NAME is null"
				  << std::flush;
		return -1;
	}

	const char *resource_root = std::getenv("DOCUMENT_ROOT");
	if( resource_root == nullptr )
	{
		std::cout << "HTTP/1.1 500 Internal Server Error\r\n"
					 "Content-length: 25\r\n"
					 "\r\n"
					 "500 DOCUMENT_ROOT is null"
				  << std::flush;
		return -1;
	}

	auto a_file_name = std::string(resource_root) + "upload/" + file_name;
	std::fstream file(a_file_name, std::ios_base::out);

	if( not file.is_open() )
	{
		std::cout << "HTTP/1.1 500 Internal Server Error\r\n"
					 "Content-Type: text/plain; charset=utf-8\r\n"
					 "Content-length: " << (20 + a_file_name.size()) << "\r\n"
					 "\r\n"
					 "file '" << a_file_name << "' open failed."
				  << std::flush;
		return -1;
	}

	const char *str_content_length = std::getenv("HTTP_CONTENT_LENGTH");
	if( str_content_length == nullptr )
	{
		std::cout << "HTTP/1.1 421 Internal Server Error\r\n"
					 "Content-length: 31\r\n"
					 "\r\n"
					 "421 HTTP_CONTENT_LENGTH is null"
				  << std::flush;
		return -1;
	}

	std::size_t content_length = 0;
	std::sscanf(str_content_length, "%zu", &content_length);

	while( content_length > 0 )
	{
		char buf[1024] = {0};
		int s = fread(buf, 1, content_length>1023?1023:content_length, stdin);

		content_length -= s;
		file.write(buf, s);
	}

	file.close();

	const char *protocl = std::getenv("SERVER_PROTOCOL");
	if( protocl == nullptr )
		protocl = "HTTP/1.1";

	std::cout << protocl << " 200 OK\r\n"
				 "Content-Length: 8\r\n"
				 "\r\n"
				 "success.";
	return 0;
}
