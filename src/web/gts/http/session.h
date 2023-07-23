#ifndef GTS_HTTP_SESSION_H
#define GTS_HTTP_SESSION_H

#include <gts/http/container.h>

namespace gts { namespace http
{

class session_impl;

using session_attribute  = std::pair<std::string, rttr::variant>;
using session_attributes = std::unordered_map<std::string, rttr::variant>;

class GTSWEB_API session
{
	GTS_DISABLE_COPY_MOVE(session)

public:
	explicit session(uint64_t seconds = 0);
	~session();

public:
	std::string id() const;
	uint64_t create_time() const;
	bool is_valid() const;

public:
	rttr::variant attribute(const std::string &key) const;
	rttr::variant attribute_or(const std::string &key, const rttr::variant &deft_value) const;
	rttr::variant attribute_or(const std::string &key, rttr::variant &&deft_value = {}) const;

public:
	session &set_attribute(const std::string &key, const rttr::variant &value);
	session &set_attribute(const std::string &key, rttr::variant &&value);

	template <typename...Args>
	session &set_attribute(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);

	session &unset_attribute(const std::string &key);

public:
	std::size_t attribute_count() const;
	session_attributes attributes() const;
	string_list attribute_key_list() const;
	std::set<std::string> attribute_key_set() const;

public:
	uint64_t lifecycle() const;
	session &set_lifecycle(uint64_t seconds = 0);
	void invalidate();

public:
	static std::shared_ptr<session> make_shared(uint64_t seconds = 0);
	static std::shared_ptr<session> get(const std::string &id);
	static void set(session *obj);

public:
	static void set_global_lifecycle(uint64_t seconds);
	static uint64_t global_lifecycle();

private:
	session_impl *m_impl;
};

using session_ptr = std::shared_ptr<session>;

template <class Sesn = session>
GTSWEB_API std::shared_ptr<Sesn> make_session(uint64_t seconds = 0)
{
	static_assert(gts_is_base_of(session, Sesn),
	"The template argument 'Sesn' must be a 'gts::http::session' or derived class of 'gts::http::session'.");

	auto obj = new Sesn(seconds); obj->set(obj);
	return std::dynamic_pointer_cast<Sesn>(session::get(obj->id()));
}

}} //namespace gts::http


#endif //GTS_HTTP_SESSION_H
