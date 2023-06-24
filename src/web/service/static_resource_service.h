#ifndef STATIC_RESOURCE_SERVICE_H
#define STATIC_RESOURCE_SERVICE_H

#include "gts/gts_global.h"
#include <fstream>

namespace gts { namespace web
{

class service_io;

class GTS_DECL_HIDDEN static_resource_service
{
	GTS_DISABLE_COPY_MOVE(static_resource_service)

public:
	explicit static_resource_service(service_io &sio);
	void call();

public:
	void default_transfer();
	void range_transfer(const std::string &range_str);

private:
	struct range_value
	{
		std::string cr_line;
		std::size_t begin, end, size;
	};

private:
	bool range_parsing(const std::string &range_str, std::size_t &begin, std::size_t &end, std::size_t &size);
	void send_range(const std::string &boundary, const std::string &ct_line, std::list<range_value> &range_value_queue);

private:
	std::size_t tcp_ip_buffer_size() const;

private:
	service_io &m_sio;
	std::fstream m_file;
};

}} //namespace gts::web


#endif //STATIC_RESOURCE_SERVICE_H
