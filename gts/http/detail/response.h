#ifndef GTS_HTTP_DETAIL_RESPONSE_H
#define GTS_HTTP_DETAIL_RESPONSE_H

namespace gts { namespace http
{

template <typename...Args>
response &response::set_header(std::string key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_header(std::move(key), fmt::format(value_fmt, std::forward<Args>(args)...));
}

template <typename...Args>
response &response::set_cookie(std::string key, fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_cookie(std::move(key), http::cookie::from(value_fmt, std::forward<Args>(args)...));
}

template <typename T>
T response::header(const std::string &key) const noexcept(false) {
	return header(key).get<T>();
}

template <typename T, typename U>
T response::header_or(const std::string &key, T deft_value) const noexcept {
	return header_or(key, http::value(std::move(deft_value))).get<T>();
}

template <typename T>
T response::cookie(const std::string &key) const noexcept(false) {
	return cookie(key).get<T>();
}

template <typename T, typename U>
T response::cookie_or(const std::string &key, T deft_value) const noexcept {
	return cookie_or(key, http::cookie(std::move(deft_value))).get<T>();
}

inline response &response::write_default(int status) {
	return set_status(status).write_default();
}

inline response &response::write() {
	return write(0,nullptr);
}

inline response &response::write(const std::string &body) {
	return write(body.size(), body.c_str());
}

inline response &response::write(const char *body) {
	return write(strlen(body), body);
}

template <typename...Args>
response &response::write(fmt::format_string<Args...> value_fmt, Args&&...args)
{
	auto body = fmt::format(value_fmt, std::forward<Args>(args)...);
	return write(body.size(), body.c_str());
}

template <typename T, typename U>
response &response::write(T &&value) {
	return write("{}", value);
}

template <typename...Args>
response &response::set_chunk_attribute(fmt::format_string<Args...> value_fmt, Args&&...args) {
	return set_chunk_attribute(fmt::format(value_fmt, std::forward<Args>(args)...));
}

inline response &response::write_file(const std::string &file_name, const range &_range) {
	return write_file(file_name, range_vector{_range});
}

inline response &response::write_file(const std::string &file_name, std::size_t begin, std::size_t end) {
	return write_file(file_name, range_vector{{begin, end}});
}

}} //namespace gts { namespace http


#endif //GTS_HTTP_DETAIL_RESPONSE_H
