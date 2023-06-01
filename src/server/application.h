#ifndef APPLICATION_H
#define APPLICATION_H

#include "gts/settings.h"
#include <asio.hpp>
#include <list>

namespace gts
{

#define gts_app  gts::applictaion::instance()

class applictaion
{
	DISABLE_COPY(applictaion)

public:
	applictaion(int argc, const char *argv[]);
	~applictaion();

public:
	static applictaion &instance();
	asio::io_context &io_context();

public:
	std::list<std::string> args() const;

public:
	int exec();
	void exit(int code);
};

} //namespace gts


#endif //APPLICATION_H
