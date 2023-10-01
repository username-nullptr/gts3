
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include "cookies.h"

namespace gts { namespace http
{

cookie::cookie()
{
	m_attributes.emplace("Path", "/");
}

cookie::cookie(cookie &&other) :
	_vbase(std::move(other))
{
	m_attributes = std::move(other.m_attributes);
}

cookie &cookie::operator=(cookie &&other)
{
	_vbase::operator=(static_cast<http::value&&>(other));
	m_attributes = std::move(other.m_attributes);
	return *this;
}

std::string cookie::domain() const
{
	auto it = m_attributes.find("Domain");
	return it == m_attributes.end()? "" : it->second;
}

std::string cookie::path() const
{
	auto it = m_attributes.find("Path");
	return it == m_attributes.end()? "" : it->second;
}

std::size_t cookie::size() const
{
	auto it = m_attributes.find("Size");
	return it == m_attributes.end()? 0 : it->second.get<std::size_t>();
}

uint64_t cookie::expires() const
{
	auto it = m_attributes.find("Expires"); //GMT
	return it == m_attributes.end()? 0 : it->second.get<uint64_t>();
}

uint64_t cookie::max_age() const
{
	auto it = m_attributes.find("Max-age");
	return it == m_attributes.end()? 0 : it->second.get<uint64_t>();
}

bool cookie::http_only() const
{
	auto it = m_attributes.find("HttpOnly");
	return it == m_attributes.end()? false : it->second.to_bool();
}

bool cookie::secure() const
{
	auto it = m_attributes.find("Secure");
	return it == m_attributes.end()? false : it->second.to_bool();
}

std::string cookie::same_site() const
{
	auto it = m_attributes.find("SameSite");
	return it == m_attributes.end()? "" : it->second;
}

std::string cookie::priority() const
{
	auto it = m_attributes.find("Priority");
	return it == m_attributes.end()? "" : it->second;
}

cookie &cookie::set_domain(std::string domain)
{
	m_attributes["Domain"] = std::move(domain);
	return *this;
}

cookie &cookie::set_path(std::string path)
{
	m_attributes["Path"] = std::move(path);
	return *this;
}

cookie &cookie::set_size(std::size_t size)
{
	m_attributes["Size"].set_value(size);
	return *this;
}

cookie &cookie::set_expires(uint64_t seconds)
{
	m_attributes.erase("Max-age");
	m_attributes["Expires"].set_value(seconds);
	return *this;
}

cookie &cookie::set_max_age(uint64_t seconds)
{
	m_attributes.erase("Expires");
	m_attributes["Max-age"].set_value(seconds);
	return *this;
}

cookie &cookie::set_http_only(bool flag)
{
	m_attributes["HttpOnly"].set_value(flag);
	return *this;
}

cookie &cookie::set_secure(bool flag)
{
	m_attributes["Secure"].set_value(flag);
	return *this;
}

cookie &cookie::same_site(std::string sst)
{
	m_attributes["SameSite"] = std::move(sst);
	return *this;
}

cookie &cookie::priority(std::string pt)
{
	m_attributes["Priority"] = std::move(pt);
	return *this;
}

cookie_attributes &cookie::attributes()
{
	return m_attributes;
}

const cookie_attributes &cookie::attributes() const
{
	return m_attributes;
}

cookie &cookie::set_attribute(std::string key, std::string value)
{
	m_attributes[std::move(key)] = std::move(value);
	return *this;
}

cookie &cookie::unset_attribute(const std::string &key)
{
	m_attributes.erase(key);
	return *this;
}

}} //namespace gts::http
