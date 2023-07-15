#include "cookies.h"

namespace gts { namespace http
{

class GTS_DECL_HIDDEN cookie_value_impl
{
public:
};

/*--------------------------------------------------------------------------------------------------------------------------------*/

cookie_attribute::cookie_attribute(cookie_attribute &&other) :
	value(std::move(other)),
	m_impl(other.m_impl)
{
	other.m_impl = nullptr;
}

cookie_attribute::~cookie_attribute()
{
	if( m_impl )
		delete m_impl;
}

cookie_attribute cookie_attribute::operator=(cookie_attribute &&other)
{
	if( m_impl )
		delete m_impl;
	m_impl = other.m_impl;
	other.m_impl = nullptr;
	return *this;
}

}} //namespace gts::http
