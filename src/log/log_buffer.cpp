#include "log_buffer.h"
#include <thread>

namespace gts { namespace log
{

extern void message_handler(log_buffer::type type, const log_buffer::context &runtime_context, const std::string &msg);

log_buffer::~log_buffer()
{
	if( m_data == nullptr )
		return ;

#ifndef __NO_DEBUG__
	else if( not m_data->buffer.empty() )
		message_handler(m_data->type, m_data->context, m_data->buffer);
#endif //__NO_DEBUG__
	delete m_data;
}

}} //namespace gts::log
