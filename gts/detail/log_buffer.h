#ifndef GTS_DETAIL_LOG_BUFFER_H
#define GTS_DETAIL_LOG_BUFFER_H

namespace gts
{

template <typename T>
inline log_buffer &log_buffer::write(T &&msg)
{
#ifndef __NO_DEBUG__
	m_data->buffer += fmt::format("{} ", std::forward<T>(msg));
#endif //__NO_DEBUG__
	return *this;
}

template <typename T>
inline log_buffer &log_buffer::operator<<(T &&msg)
{
	return write(std::forward<T>(msg));
}

} //namespace gts


#endif //GTS_DETAIL_LOG_BUFFER_H
