#ifndef GTS_HTTP_DETAIL_VALUE_H
#define GTS_HTTP_DETAIL_VALUE_H

namespace gts { namespace http
{

inline value::value(const std::string &str) :
	base_type(str)
{

}

inline value::value(const http::value &other) :
	base_type(other)
{

}

inline value::value(std::string &&str) noexcept :
	base_type(std::move(str))
{

}

inline value::value(http::value &&other) noexcept :
	base_type(std::move(other))
{

}

inline value::value(const char *str, size_type len) :
	base_type(str,len)
{

}

template <typename T, typename U>
inline value::value(T &&v) :
	base_type(fmt::format("{}", std::forward<T>(v)))
{

}

inline bool value::to_bool() const
{
	return get<bool>();
}

inline int32_t value::to_int() const
{
	return get<int32_t>();
}

inline uint32_t value::to_uint() const
{
	return get<uint32_t>();
}

inline int64_t value::to_long() const
{
	return get<int64_t>();
}

inline uint64_t value::to_ulong() const
{
	return get<uint64_t>();
}

inline float value::to_float() const
{
	return get<float>();
}

inline double value::to_double() const
{
	return get<double>();
}

inline long double value::to_ldouble() const
{
	return get<long double>();
}

template <typename T, typename _GTEI_0>
bool value::get() const
{
	return gts::stob(*this);
}

template <typename T, typename _GTEI_0>
signed char value::get() const
{
	return gts::stoi8(*this);
}

template <typename T, typename _GTEI_0>
unsigned char value::get() const
{
	return gts::stoui8(*this);
}

template <typename T, typename _GTEI_0>
short value::get() const
{
	return gts::stoi16(*this);
}

template <typename T, typename _GTEI_0>
unsigned short value::get() const
{
	return gts::stoui16(*this);
}

template <typename T, typename _GTEI_0>
int value::get() const
{
	return gts::stoi32(*this);
}

template <typename T, typename _GTEI_0>
unsigned int value::get() const
{
	return gts::stoui32(*this);
}

template <typename T, typename _GTEI_0>
long value::get() const
{
	return gts::stoi64(*this);
}

template <typename T, typename _GTEI_0>
unsigned long value::get() const
{
	return gts::stoui64(*this);
}

template <typename T, typename _GTEI_0>
long long value::get() const
{
	return gts::stoui64(*this);
}

template <typename T, typename _GTEI_0>
unsigned long long value::get() const
{
	return gts::stoui64(*this);
}

template <typename T, typename _GTEI_0>
float value::get() const
{
	return gts::stof(*this);
}

template <typename T, typename _GTEI_0>
double value::get() const
{
	return gts::stod(*this);
}

template <typename T, typename _GTEI_0>
long double value::get() const
{
	return gts::stold(*this);
}

template <typename T, typename _GTEI_0>
std::string &value::get()
{
	return *this;
}

template <typename T, typename _GTEI_0>
const std::string &value::get() const
{
	return *this;
}

template <typename T, typename _GTEI_0>
T value::get() const
{
	return static_cast<T>(get<int>());
}

inline value &value::set_value(const std::string &v)
{
	operator=(v);
	return *this;
}

inline value &value::set_value(std::string &&v)
{
	operator=(std::move(v));
	return *this;
}

template <typename...Args>
value &value::set_value(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	operator=(fmt::format(fmt_value, std::forward<Args>(args)...));
	return *this;
}

template <typename T, typename U>
value &value::set_value(T &&v)
{
	set_value("{}", std::forward<T>(v));
	return *this;
}

template <typename...Args>
value value::from(fmt::format_string<Args...> fmt_value, Args&&...args)
{
	value hv;
	hv.set_value(fmt::format(fmt_value, std::forward<Args>(args)...));
	return hv;
}

template <typename T, typename U>
value value::from(T &&v)
{
	value hv;
	hv.set_value("{}", std::forward<T>(v));
	return hv;
}

template <typename T, typename _GTEI_0>
value &value::operator=(T value)
{
	operator=(std::to_string(value));
	return *this;
}

inline value &value::operator=(const std::string &other)
{
	base_type::operator=(other);
	return *this;
}

inline value &value::operator=(const http::value &other)
{
	base_type::operator=(other);
	return *this;
}

inline value &value::operator=(std::string &&other)
{
	base_type::operator=(std::move(other));
	return *this;
}

inline value &value::operator=(http::value &&other)
{
	base_type::operator=(std::move(other));
	return *this;
}

}} //namespace gts::http

namespace std
{

inline size_t hash<gts::http::value>::operator()(const gts::http::value &v) const noexcept
{
	return hash<std::string>()(v);
}

} //namespace std


#endif //GTS_HTTP_DETAIL_VALUE_H
