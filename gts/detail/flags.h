#ifndef GTS_DETAIL_FLAGS_H
#define GTS_DETAIL_FLAGS_H

namespace gts
{

template <typename Enum>
constexpr inline flags<Enum>::flags(Enum f) noexcept :
	m_value(f)
{

}

template <typename Enum>
constexpr inline flags<Enum>::flags(const flags &other) :
	m_value(other.m_value)
{

}

template <typename Enum>
constexpr inline flags<Enum>::flags(std::initializer_list<Enum> flags) noexcept :
	m_value(initializer_list_helper(flags.begin(), flags.end()))
{

}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator=(const flags<Enum> &other)
{
	m_value = other.m_value;
	return *this;
}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator&=(int mask) noexcept
{
	m_value &= mask;
	return *this;
}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator&=(unsigned int mask) noexcept
{
	m_value &= mask;
	return *this;
}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator&=(Enum mask) noexcept
{
	m_value &= mask;
	return *this;
}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator|=(flags f) noexcept
{
	m_value |= f.m_value;
	return *this;
}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator|=(Enum f) noexcept
{
	m_value |= f;
	return *this;
}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator^=(flags<Enum> f) noexcept
{
	m_value ^= f.m_value;
	return *this;
}

template <typename Enum>
const inline flags<Enum> &flags<Enum>::operator^=(Enum f) noexcept
{
	m_value ^= f;
	return *this;
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator|(flags<Enum> f) const noexcept
{
	return flags<Enum>( static_cast<Enum>(m_value | f.m_value) );
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator|(Enum f) const noexcept
{
	return flags<Enum>( static_cast<Enum>(m_value | f) );
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator^(flags<Enum> f) const noexcept
{
	return flags<Enum>( static_cast<Enum>(m_value ^ f.m_value) );
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator^(Enum f) const noexcept
{
	return flags<Enum>( static_cast<Enum>(m_value ^ f) );
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator&(int mask) const noexcept
{
	return flags<Enum>( static_cast<Enum>(m_value & mask));
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator&(unsigned int mask) const noexcept
{
	return flags<Enum>( static_cast<Enum>(m_value & mask));
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator&(Enum f) const noexcept
{
	return flags<Enum>( static_cast<Enum>(m_value & f) );
}

template <typename Enum>
constexpr inline flags<Enum> flags<Enum>::operator~() const noexcept
{
	return flags<Enum>( static_cast<Enum>(~m_value) );
}

template <typename Enum>
constexpr inline bool flags<Enum>::operator!() const noexcept
{
	return not m_value;
}

template <typename Enum>
constexpr inline flags<Enum>::operator int() const noexcept
{
	return m_value;
}

template <typename Enum>
constexpr inline bool flags<Enum>::testFlag(Enum f) const noexcept
{
	return (m_value & f) == f and (f != 0 or m_value == f);
}

template <typename Enum>
constexpr inline flags<Enum> &flags<Enum>::setFlag(Enum f, bool on) const noexcept
{
	return on ? (*this |= f) : (*this &= ~f);
}

template <typename Enum>
constexpr inline int flags<Enum>::initializer_list_helper(iterator it, iterator end) noexcept
{
	return (it == end ? 0 : (*it | initializer_list_helper(it + 1, end)));
}

} //namespace gts


#endif //GTS_DETAIL_FLAGS_H
