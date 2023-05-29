#ifndef WEB_GLOBAL_H
#define WEB_GLOBAL_H

#include "gts/gts_global.h"
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


#endif //WEB_GLOBAL_H
