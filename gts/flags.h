#ifndef GTS_FLAGS_H
#define GTS_FLAGS_H

#include <initializer_list>

namespace gts
{

template <typename Enum>
class flags
{
	static_assert((std::is_enum<Enum>::value),
	"flags is only usable on enumeration types.");

	static_assert((sizeof(Enum) <= sizeof(int)),
	"flags uses an int as storage, so an enum with underlying long long will overflow.");

public:
	using enum_type = Enum;

	constexpr inline flags() noexcept : i(0) {}
	constexpr inline flags(Enum f) noexcept : i(f) {}
	constexpr inline flags(const flags &other) : i(other.i) {}

	constexpr inline flags(std::initializer_list<Enum> flags) noexcept
		: i(initializer_list_helper(flags.begin(), flags.end())) {}

	const inline flags &operator=(const flags &other)
	{ i = other.i; return *this; }

public:
	const inline flags &operator&=(int mask) noexcept
	{ i &= mask; return *this; }

	const inline flags &operator&=(unsigned int mask) noexcept
	{ i &= mask; return *this; }

	const inline flags &operator&=(Enum mask) noexcept
	{ i &= mask; return *this; }

	const inline flags &operator|=(flags f) noexcept
	{ i |= f.i; return *this; }

	const inline flags &operator|=(Enum f) noexcept
	{ i |= f; return *this; }

	const inline flags &operator^=(flags f) noexcept
	{ i ^= f.i; return *this; }

	const inline flags &operator^=(Enum f) noexcept
	{ i ^= f; return *this; }

	constexpr inline operator int() const noexcept
	{ return i; }

	constexpr inline flags operator|(flags f) const noexcept
	{ return flags( static_cast<Enum>(i | f.i) ); }

	constexpr inline flags operator|(Enum f) const noexcept
	{ return flags( static_cast<Enum>(i | f) ); }

	constexpr inline flags operator^(flags f) const noexcept
	{ return flags( static_cast<Enum>(i ^ f.i) ); }

	constexpr inline flags operator^(Enum f) const noexcept
	{ return flags( static_cast<Enum>(i ^ f) ); }

	constexpr inline flags operator&(int mask) const noexcept
	{ return flags( static_cast<Enum>(i & mask)); }

	constexpr inline flags operator&(unsigned int mask) const noexcept
	{ return flags( static_cast<Enum>(i & mask)); }

	constexpr inline flags operator&(Enum f) const noexcept
	{ return flags( static_cast<Enum>(i & f) ); }

	constexpr inline flags operator~() const noexcept
	{ return flags( static_cast<Enum>(~i) ); }

	constexpr inline bool operator!() const noexcept
	{ return !i; }

	constexpr inline bool testFlag(Enum f) const noexcept
	{ return (i & f) == f and (f != 0 or i == f); }

	constexpr inline flags &setFlag(Enum f, bool on = true) const noexcept
	{ return on ? (*this |= f) : (*this &= ~f); }

private:
	using iterator = typename std::initializer_list<Enum>::const_iterator;
	constexpr static inline int initializer_list_helper(iterator it, iterator end) noexcept {
		return (it == end ? 0 : (*it | initializer_list_helper(it + 1, end)));
	}
	int i;
};

/*------------------------------------------------------------*/

#define GTS_DECLARE_FLAGS(_flags, _enum)  using _flags = flags<_enum>;

#define GTS_DECLARE_OPERATORS_FOR_FLAGS(_flags) \
	constexpr inline flags<_flags::enum_type> operator| \
	(_flags::enum_type f1, _flags::enum_type f2) noexcept \
	{ return flags<_flags::enum_type>(f1) | f2; } \
	constexpr inline flags<_flags::enum_type> operator| \
	(_flags::enum_type f1, flags<_flags::enum_type> f2) noexcept \
	{ return f2 | f1; }

} //namespace gts


#endif //GTS_FLAGS_H
