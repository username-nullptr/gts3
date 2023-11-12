#include <cppfilesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>

int main()
{
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

	std::string request;
	while( content_length > 0 )
	{
		char buf[1024] = {0};
		int s = std::fread(buf, 1, content_length>1023?1023:content_length, stdin);

		content_length -= s;
		request.append(buf, s);
	}
	auto pos = request.find('&');
	if( pos == std::string::npos )
	{
		std::cout << "HTTP/1.1 421 Misdirected Request\r\n"
					 "Content-Length: 7\r\n"
					 "\r\n"
					 "421 (&)"
				  << std::flush;
		return -1;
	}
	auto usrnam_str = request.substr(0, pos);
	auto passwd_str = request.substr(pos + 1);

	pos = usrnam_str.find('=');
	if( pos == std::string::npos or pos == 0 or pos == usrnam_str.size() - 1 )
	{
		std::cout << "HTTP/1.1 421 Misdirected Request\r\n"
					 "Content-Length: 15\r\n"
					 "\r\n"
					 "User name null."
				  << std::flush;
		return -1;
	}
	auto username = usrnam_str.substr(pos + 1);

	pos = passwd_str.find('=');
	if( pos == std::string::npos or pos == 0 or pos == passwd_str.size() - 1 )
	{
		std::cout << "HTTP/1.1 421 Misdirected Request\r\n"
					 "Content-Length: 14\r\n"
					 "\r\n"
					 "Password null."
				  << std::flush;
		return -1;
	}
	auto password = passwd_str.substr(pos + 1);

	if( username != "LaoFeng" or password != "JiTangLaiLou" )
	{
		std::cout << "HTTP/1.1 401 Unauthorized\r\n"
					 "Content-Length: 39\r\n"
					 "\r\n"
					 "The user name or password is incorrect."
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
	auto file_name = std::string(resource_root) + "netprotocol.html";

	if( not fs::exists(file_name) )
	{
		std::cerr << "File read error: Not exists." << std::endl;

		std::cout << "HTTP/1.1 500 Internal Server Error\r\n"
					 "Content-length: " << (4 + file_name.size()) << "\r\n"
					 "\r\n"
					 "500 " << file_name
				  << std::flush;
		return -1;
	}
	std::fstream file(file_name);

	if( not file.is_open() )
	{
		std::cerr << "File open failed." << std::endl;

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
			std::cerr << "File read error." << std::endl;
			break;
		}
		else if( size > 0 )
			std::cout << std::string(buf, size);
		else
			break;
	}
	file.close();
	return 0;
}
