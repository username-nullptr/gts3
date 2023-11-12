#include <cppfilesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

int main()
{
	FILE *log_file = std::fopen("cgi0.log", "w");
	std::fwrite("start.\n", 1, 7, log_file);

	const char *resource_root = std::getenv("DOCUMENT_ROOT");
	if( resource_root == nullptr )
	{
		const char *buf = "resource_root is null.";
		std::fwrite(buf, 1, std::strlen(buf), log_file);

		std::cout << "HTTP/1.1 500 Internal Server Error\r\n"
					 "Content-length: 25\r\n"
					 "\r\n"
					 "500 DOCUMENT_ROOT is null"
				  << std::flush;
		return -1;
	}
	auto file_name = std::string(resource_root) + "pass.html";

	if( not fs::exists(file_name) )
	{
		const char *buf = "File read error: Not exists.";
		std::fwrite(buf, 1, std::strlen(buf), log_file);

		std::cout << "HTTP/1.1 500 Internal Server Error\r\n"
					 "Content-length: " << (4 + file_name.size()) << "\r\n"
					 "\r\n"
					 "500 " << file_name
				  << std::flush;
		return -1;
	}
	std::ifstream file(file_name);

	if( not file.is_open() )
	{
		const char *buf = "File open failed.";
		std::fwrite(buf, 1, std::strlen(buf), log_file);

		std::cout << "HTTP/1.1 500 Internal Server Error\r\n"
					 "Content-length: 21\r\n"
					 "\r\n"
					 "500 file open failed."
				  << std::flush;
		return -1;
	}
	const char *protocl = std::getenv("SERVER_PROTOCOL");
	if( protocl == nullptr )
		protocl = "HTTP/1.1";

	std::cout << protocl << " 200 OK\r\n"
				 "Content-Type: text/html; charset=utf-8\r\n"
				 "Content-Length: " << fs::file_size(file_name) << "\r\n"
				 "\r\n";

	char buf[1024] = "";
	while( not file.eof() )
	{
		file.read(buf, 1024);
		auto size = file.gcount();

		if( size < 0 )
		{
			const char *_buf = "File read error.";
			std::fwrite(_buf, 1, std::strlen(_buf), log_file);
			break;
		}
		else if( size > 0 )
			std::cout << std::string(buf, size);
		else
			break;
	}
	file.close();
	std::fclose(log_file);
	return 0;
}
