#ifndef GTS_APPLICATION_H
#define GTS_APPLICATION_H

#include <gts/gts_global.h>
#include <gts/formatter.h>
#include <cppoptional>

namespace gts { namespace app
{

GTSCORE_API std::string instance_name();

GTSCORE_API std::string file_path();
GTSCORE_API std::string dir_path();

GTSCORE_API bool set_current_directory(const std::string &path);
GTSCORE_API std::string current_directory();

GTSCORE_API std::string absolute_path(const std::string &path);
GTSCORE_API bool is_absolute_path(const std::string &path);

GTSCORE_API cpp::optional<std::string> getenv(const std::string &key);
GTSCORE_API std::map<std::string, std::string> getenvs();

GTSCORE_API bool setenv(const std::string &key, const std::string &value, bool overwrite = true);
GTSCORE_API bool unsetenv(const std::string &key);

template <typename...Args>
bool setenv(const std::string &key, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return setenv(key, fmt::format(fmt_value, std::forward<Args>(args)...));
}

template <typename...Args>
bool setenv(const std::string &key, bool overwrite, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return setenv(key, fmt::format(fmt_value, std::forward<Args>(args)...), overwrite);
}

template <typename T, typename _U = enable_if_t<not gts_is_same(decay_t<T>, std::string) and
												not gts_is_same(T, const char*) and
												not gts_is_same(T, char*), int>>
bool setenv(const std::string &key, T &&value, bool overwrite = true) {
	return setenv(key, fmt::format("{}", std::forward<T>(value)), overwrite);
}

}} //namespace gts::app


#endif //GTS_APPLICATION_H
