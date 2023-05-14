#ifndef REQUEST_H
#define REQUEST_H

#include "protocol.h"
#include <asio.hpp>

using namespace asio::ip;

namespace gts::web::http
{

class parser;

class GTS_DECL_HIDDEN request
{
	DISABLE_COPY(request)
	friend class parser;

public:
	request();
	~request();

public:
	std::string method() const;
	std::string version() const;
	std::string path() const;

public:
	std::string parameters_string() const;
	const http::parameters &parameters() const;
	const http::headers &headers() const;

public:
	const std::string &body() const;

public:
	bool keep_alive() const;
	bool support_gzip() const;
	bool is_valid() const;

private:
	void finish();

private:
	std::string m_method;
	std::string m_version;
	std::string m_path;
	std::string m_parameters_string;
	http::parameters m_parameters;
	http::headers m_headers;
	std::string m_body;

private:
	bool m_keep_alive = true;
	bool m_gzip = false;
};

} //namespace gts::web::http


#endif //REQUEST_H
