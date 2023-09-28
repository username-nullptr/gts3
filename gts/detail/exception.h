#ifndef GTS_DETAIL_EXCEPTION_H
#define GTS_DETAIL_EXCEPTION_H

namespace gts
{

inline exception::exception(std::string what) :
	m_what(std::move(what))
{

}

template <typename...Args>
exception::exception(fmt::format_string<Args...> fmt_value, Args&&...args) :
	m_what(fmt::format(fmt_value, std::forward<Args>(args)...))
{

}

inline const char *exception::what() const _GLIBCXX_NOTHROW
{
	return m_what.c_str();
}

#if GTS_CPLUSPLUS < 201703L

inline exception::exception(exception &&other) noexcept :
	m_what(std::move(other.m_what))
{

}

#endif

} //namespace gts


#endif //GTS_DETAIL_EXCEPTION_H
