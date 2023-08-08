#ifndef GTS_HTTP_SESSION_H
#define GTS_HTTP_SESSION_H

#include <gts/http/container.h>
#include <gts/exception.h>

namespace gts { namespace http
{

class session_impl;

using session_attribute  = std::pair<std::string, rttr::variant>;
using session_attributes = std::unordered_map<std::string, rttr::variant>;

class GTSWEB_API session
{
	GTS_DISABLE_COPY_MOVE(session)
	using duration = std::chrono::seconds;

	template <typename T>
	using not_variant_t = enable_if_t<not gts_is_dsame(T, rttr::variant), int>;

public:
	explicit session(const duration &seconds = duration(0));
	virtual ~session();

public:
	std::string id() const;
	uint64_t create_time() const;
	bool is_valid() const;

public:
	rttr::variant attribute(const std::string &key) const;
	rttr::variant attribute_or(const std::string &key, rttr::variant deft_value = {}) const;

	template <typename T>
	T attribute(const std::string &key) const;

	template <typename T, typename U = not_variant_t<T>>
	T attribute_or(const std::string &key, T deft_value) const;

public:
	session &set_attribute(std::string key, rttr::variant value);

	template <typename...Args>
	session &set_attribute(std::string key, fmt::format_string<Args...> fmt, Args&&...args);

	session &unset_attribute(const std::string &key);

public:
	std::size_t attribute_count() const;
	session_attributes attributes() const;
	string_list attribute_key_list() const;
	std::set<std::string> attribute_key_set() const;

public:
	uint64_t lifecycle() const;
	session &set_lifecycle(const duration &seconds = duration(0));
	session &expand(const duration &seconds = duration(0));
	void invalidate();

public:
	static std::shared_ptr<session> make_shared(const duration &seconds = duration(0));
	static std::shared_ptr<session> get(const std::string &id);
	static void set(session *obj);

    template <class Sesn>
	static std::shared_ptr<Sesn> get(const std::string &id);

public:
	static void set_global_lifecycle(const duration &seconds);
	static duration global_lifecycle();

private:
	session_impl *m_impl;
};

using session_ptr = std::shared_ptr<session>;

template <class Sesn = session>
GTSWEB_API std::shared_ptr<Sesn> make_session(const std::chrono::seconds &seconds = std::chrono::seconds(0))
{
	static_assert(gts_is_base_of(session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");

	auto obj = new Sesn(seconds); obj->set(obj);
	return std::dynamic_pointer_cast<Sesn>(session::get(obj->id()));
}

template <typename T>
T session::attribute(const std::string &key) const {
	return attribute(key).convert<T>();
}

template <typename T, typename U>
T session::attribute_or(const std::string &key, T deft_value) const {
	return attribute_or(key, rttr::variant(std::move(deft_value))).convert<T>();
}

template <typename...Args>
session &session::set_attribute(std::string key, fmt::format_string<Args...> fmt_value, Args&&...args) {
	return set_attribute(std::move(key), fmt::format(fmt_value, std::forward<Args>(args)...));
}

template <class Sesn>
std::shared_ptr<Sesn> session::get(const std::string &id)
{
	static_assert(gts_is_base_of(http::session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");

    auto ptr = get(id);
    if( ptr == nullptr )
        return std::shared_ptr<Sesn>();

    auto dy_ptr = std::dynamic_pointer_cast<Sesn>(ptr);
    if( dy_ptr == nullptr )
        throw exception("gts::http::session::get<{}>: The type of 'session' is incorrect.", type_name<Sesn>());
    return dy_ptr;
}

}} //namespace gts::http


#endif //GTS_HTTP_SESSION_H
