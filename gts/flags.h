#ifndef GTS_FLAGS_H
#define GTS_FLAGS_H

#include <initializer_list>
#include <gts/utility.h>

namespace gts
{

template <typename Enum>
class flags
{
	static_assert((gts_is_enum(Enum)),
	"flags is only usable on enumeration types.");

	static_assert((sizeof(Enum) <= sizeof(int)),
	"flags uses an int as storage, so an enum with underlying long long will overflow.");

public:
	using enum_type = Enum;
	constexpr flags() noexcept = default;
	constexpr flags(Enum f) noexcept;
	constexpr flags(const flags &other);
	constexpr flags(std::initializer_list<Enum> flags) noexcept;
	const flags &operator=(const flags &other);

public:
	const flags &operator&=(int mask) noexcept;
	const flags &operator&=(unsigned int mask) noexcept;
	const flags &operator&=(Enum mask) noexcept;
	const flags &operator|=(flags f) noexcept;
	const flags &operator|=(Enum f) noexcept;
	const flags &operator^=(flags f) noexcept;
	const flags &operator^=(Enum f) noexcept;

public:
	constexpr flags operator|(flags f) const noexcept;
	constexpr flags operator|(Enum f) const noexcept;
	constexpr flags operator^(flags f) const noexcept;
	constexpr flags operator^(Enum f) const noexcept;
	constexpr flags operator&(int mask) const noexcept;
	constexpr flags operator&(unsigned int mask) const noexcept;
	constexpr flags operator&(Enum f) const noexcept;
	constexpr flags operator~() const noexcept;
	constexpr bool operator!() const noexcept;

public:
	constexpr operator int() const noexcept;
	constexpr bool testFlag(Enum f) const noexcept;
	constexpr flags &setFlag(Enum f, bool on = true) const noexcept;

private:
	using iterator = typename std::initializer_list<Enum>::const_iterator;
	constexpr static int initializer_list_helper(iterator it, iterator end) noexcept;
	int m_value = 0;
};

} //namespace gts

#include <gts/detail/flags.h>

#define GTS_DECLARE_FLAGS(_flags, _enum)  using _flags = flags<_enum>;

#define GTS_DECLARE_OPERATORS_FOR_FLAGS(_flags) \
	constexpr inline flags<_flags::enum_type> operator| \
	(_flags::enum_type f1, _flags::enum_type f2) noexcept \
	{ return flags<_flags::enum_type>(f1) | f2; } \
	constexpr inline flags<_flags::enum_type> operator| \
	(_flags::enum_type f1, flags<_flags::enum_type> f2) noexcept \
	{ return f2 | f1; }

#endif //GTS_FLAGS_H
