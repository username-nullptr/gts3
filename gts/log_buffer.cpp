#include "log_buffer.h"
#include <thread>

namespace gts { namespace log
{

log_buffer::context::context(context &&other) {
	operator=(std::move(other));
}

log_buffer::context &log_buffer::context::operator=(context &&other)
{
	time     = std::move(other.time);
	category = std::move(other.category);
	file     = other.file;
	func     = other.func;
	line     = other.line;
	return *this;
}

log_buffer::log_buffer(type t) : m_data(new data)
{
	m_data->type = t;
}

log_buffer::log_buffer(const log_buffer &other) : m_data(new data) {
	operator=(other);
}

log_buffer::log_buffer(log_buffer &&other)
{
	m_data = other.m_data;
	other.m_data = nullptr;
}

log_buffer &log_buffer::operator=(const log_buffer &other)
{
	*m_data = *other.m_data;
	return *this;
}

log_buffer &log_buffer::operator=(log_buffer &&other)
{
	if( m_data )
		delete m_data;
	m_data = other.m_data;
	other.m_data = nullptr;
	return *this;
}

extern void message_handler(log_buffer::type type, log_buffer::context &&runtime_context, std::string &&msg);

log_buffer::~log_buffer()
{
	if( m_data == nullptr )
		return ;

#ifndef __NO_DEBUG__
	else if( not m_data->buffer.empty() )
		message_handler(m_data->type, std::move(m_data->context), std::move(m_data->buffer));
#endif //__NO_DEBUG__
	delete m_data;
}

}} //namespace gts::log
