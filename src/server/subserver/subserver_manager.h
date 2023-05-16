#ifndef SUBSERVER_MANAGER_H
#define SUBSERVER_MANAGER_H

#include "gts/gts_global.h"

namespace gts { namespace subserver
{

#define gts_sm  gts::subserver::manager::instance()

class manager
{
	DISABLE_COPY(manager)

public:
	manager();
	~manager();

public:
	static manager &instance();

public:
	void start_all();
	void stop_all();

public:
	std::string view_all_server() const;
	std::string view_running_server() const;
};

}} //namespace gts::subserver


#endif //SUBSERVER_MANAGER_H
