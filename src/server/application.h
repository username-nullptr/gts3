#ifndef APPLICATION_H
#define APPLICATION_H

#include "gts/string_list.h"
#include "gts/settings.h"
#include <asio.hpp>
#include <list>

namespace gts
{

#define gts_app  gts::applictaion::instance()

class applictaion
{
	GTS_DISABLE_COPY_MOVE(applictaion)

public:
	applictaion(int argc, const char *argv[]);
	~applictaion();

public:
	static applictaion &instance();
	asio::io_context &io_context();

public:
	string_list args() const;

public:
	int exec();
	void exit(int code);
};

} //namespace gts


#endif //APPLICATION_H
