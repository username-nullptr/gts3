#ifndef GLOBAL_H
#define GLOBAL_H

#include "gts_global.h"
#include <asio.hpp>
#include <vector>

using namespace asio::ip;

namespace gts { namespace web
{

GTS_DECL_HIDDEN asio::io_context &io_context();

template <typename T> inline
void delete_later(T *obj)
{
	io_context().post([obj]{
		delete obj;
	});
}

}} //namespace gts::web


#endif //GLOBAL_H
