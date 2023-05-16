#ifndef INTERACTION_H
#define INTERACTION_H

#include "gts/gts_global.h"
#include <functional>
#include <string>

namespace gts { namespace cmdline
{

class interaction_private;

class interaction
{
	DISABLE_COPY(interaction)

public:
	explicit interaction(bool is_server = true);
	~interaction();

public:
	bool open();
	void close();
	bool is_open() const;

public:
	int read(char *buf, int len, int timeout = -1);
	int write(const char *buf, int len, int timeout = -1);

public:
	void async_read(char *buf, int len, std::function<void(int len)> call_back);
	void async_write(char *buf, int len, std::function<void(int len)> call_back);
	void cancel();

private:
	friend class interaction_private;
	interaction_private *d_ptr;
};

}} //namespace gts::cmdline


#endif //INTERACTION_H
