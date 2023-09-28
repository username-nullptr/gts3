#ifndef GTS_DBI_DETAIL_ERROR_H
#define GTS_DBI_DETAIL_ERROR_H

namespace gts { namespace dbi
{

inline error_code::error_code(int value, std::string message) :
	m_value(value), m_message(std::move(message))
{

}

inline error_code::error_code(const error_code &other) noexcept :
	m_value(other.value()), m_message(other.m_message)
{

}

inline error_code::error_code(error_code &&other) noexcept :
	m_value(other.value()), m_message(std::move(other.m_message))
{

}

inline int error_code::value() const
{
	return m_value;
}

inline std::string message() const
{
	return m_message;
}

inline error_code::operator bool() const
{
	return value() != 0;
}

inline error_code &error_code::operator=(const error_code &other) noexcept
{
	m_value = other.m_value;
	m_message = other.m_message;
	return *this;
}

inline error_code &error_code::operator=(error_code &&other) noexcept
{
	m_value = other.value();
	m_message = std::move(other.m_message);
	return *this;
}

inline exception::exception(int value, std::string message) :
	error_code(value, std::move(message))
{

}

inline exception::exception(const error_code &other) noexcept :
	error_code(other)
{

}

inline exception::exception(error_code &&other) noexcept :
	error_code(std::move(other))
{

}

template <typename...Args>
exception::exception(int value, fmt::format_string<Args...> fmt_value, Args&&...args) :
	error_code(value, fmt::format(fmt_value, std::forward<Args>(args)...))
{

}

inline const char *exception::what() const _GLIBCXX_USE_NOEXCEPT
{
	return m_message.c_str();
}

#if GTS_CPLUSPLUS < 201703L

inline exception::exception(exception &&other) noexcept
{
	m_value = other.m_value;
	m_message = std::move(other.m_message);
}

#endif

}} //namespace gts::dbi


#endif //GTS_DBI_DETAIL_ERROR_H
