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

public: // Must be created in the heap. (new session)
	explicit session(uint64_t seconds = 0);
	~session();

public:
	std::string id() const;
	uint64_t create_time() const;
	bool is_valid() const;

public:
	const session_attributes &attributes() const;
	session_attributes &attributes();

	const rttr::variant &attribute(const std::string &key) const;
	rttr::variant attribute_or(const std::string &key, const rttr::variant &deft_value) const;
	rttr::variant attribute_or(const std::string &key, rttr::variant &&deft_value = {}) const;

public:
	session &set_attribute(const std::string &key, const rttr::variant &value);
	session &set_attribute(const std::string &key, rttr::variant &&value);

	template <typename...Args>
	session &set_attribute(const std::string &key, fmt::format_string<Args...> fmt, Args&&...args);

	session &unset_attribute(const std::string &key);

public:
	uint64_t lifecycle() const;
	session &set_lifecycle(uint64_t seconds = 0);
	void invalidate();

public:
	static std::shared_ptr<session> make_shared(uint64_t seconds = 0);
	static std::shared_ptr<session> get(const std::string &id);
	static void set_global_lifecycle(uint64_t seconds);
	static uint64_t global_lifecycle();

private:
	session_impl *m_impl;
};

using session_ptr = std::shared_ptr<session>;

}} //namespace gts::http


#endif //GTS_HTTP_SESSION_H
