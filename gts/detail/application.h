#ifndef GTS_DETAIL_APPLICATION_H
#define GTS_DETAIL_APPLICATION_H

namespace gts
{

template <typename...Args>
bool setenv(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	return setenv(key, fmt::format(fmt_value, std::forward<Args>(args)...));
}

template <typename...Args>
bool setenv(const std::string &key, bool overwrite, fmt::format_string<Args...> fmt_value, Args&&...args)
{
	return setenv(key, fmt::format(fmt_value, std::forward<Args>(args)...), overwrite);
}

template <typename T, typename _U>
bool setenv(const std::string &key, T &&value, bool overwrite = true)
{
	return setenv(key, fmt::format("{}", std::forward<T>(value)), overwrite);
}

} //namespace gts


#endif //GTS_DETAIL_APPLICATION_H
