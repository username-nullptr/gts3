
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

#ifndef GTS_DETAIL_REGISTRATION_H
#define GTS_DETAIL_REGISTRATION_H

#include <gts/log.h>

namespace gts
{

template <typename This, typename Func, typename _U0>
This &registration_base::register_method(const std::string &prefix, const std::string &name, Func &&func)
{
	auto addr = reinterpret_cast<const void*>(&func);
	if( g_func_set.emplace(addr).second )
		rttr::registration::method(fmt::format("gts{}.plugin{}.{}", prefix, name, addr), std::forward<Func>(func));
	else
		gts_log_fatal("gts::registration_base::{}: multiple registration.", name);
	return *reinterpret_cast<This*>(this);
}

template <typename This, typename Func, typename _U0, typename _U1>
This &registration_base::register_method(const std::string &prefix, const std::string &name, Func &&func)
{
	rttr::registration::method(fmt::format("gts{}.plugin{}.co.{}", prefix, name, g_gfs_counter++), std::forward<Func>(func));
	return *reinterpret_cast<This*>(this);
}

template <typename Func, typename _GTD_0>
registration &registration::new_connection(Func &&func, uint16_t port)
{
	if( g_func_set.emplace(reinterpret_cast<const void*>(reinterpret_cast<const char*>(&func) + port)).second )
		rttr::registration::method(port == 0? "gts.plugin.new_connection" : fmt::format("gts.plugin.new_connection.{}", port), std::forward<Func>(func));
	else
		gts_log_fatal("gts::registration::new_connection: multiple registration.");
	return *this;
}

template <typename Func, typename _GTD_0>
registration &registration::init_method(Func &&func)
{
	return register_method<registration>("", ".init", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0, int U0>
registration &registration::init_method(Func &&func)
{
	return register_method<registration>("", ".init", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0>
registration &registration::exit_method(Func &&func)
{
	return register_method<registration>("", ".exit", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0>
registration &registration::view_status_method(Func &&func)
{
	return register_method<registration>("", ".view_status", std::forward<Func>(func));
}

template <typename Class>
registration::class_<Class>::class_() :
	m_class_name(fmt::format("gts.plugin.class.{}", typeid(Class).hash_code())),
	m_class_(std::make_shared<rttr::registration::class_<Class>>(m_class_name))
{
	m_class_->constructor();
	m_type = rttr::type::get_by_name(m_class_name);
}

template <typename Class>
template <typename Return>
registration::class_<Class> &registration::class_<Class>::init_method(Return(Class::*func)(void))
{
	m_class_->method(fmt::format("init.{}", m_type.get_id()), func);
	return *this;
}

template <typename Class>
template <typename Return, typename Str, typename _GTD_0>
registration::class_<Class> &registration::class_<Class>::init_method(Return(Class::*func)(Str))
{
	m_class_->method(fmt::format("init.{}", m_type.get_id()), func);
	return *this;
}

template <typename Class>
template <typename Return>
registration::class_<Class> &registration::class_<Class>::exit_method(Return(Class::*func)(void))
{
	m_class_->method(fmt::format("exit.{}", m_type.get_id()), func);
	return *this;
}

template <typename Class>
registration::class_<Class> &registration::class_<Class>::view_status_method(std::string(Class::*func)(void))
{
	m_class_->method(fmt::format("view_status.{}", m_type.get_id()), func);
	return *this;
}

namespace extension
{

template <typename Func, typename _GTD_0>
registration &registration::init_method(Func &&func)
{
	return register_method<registration>(".extension", ".init", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0, int U0>
registration &registration::init_method(Func &&func)
{
	return register_method<registration>(".extension", ".init", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0>
registration &registration::exit_method(Func &&func)
{
	return register_method<registration>(".extension", ".exit", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0>
registration &registration::args_parsing_method(Func &&func)
{
	return register_method<registration>(".extension", ".args_parsing", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0, int U0>
registration &registration::args_parsing_method(Func &&func)
{
	return register_method<registration>(".extension", ".args_parsing", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0>
registration &registration::view_version_method(Func &&func)
{
	return register_method<registration>(".extension", ".view_version", std::forward<Func>(func));
}

template <typename Func, typename _GTD_0>
registration &registration::view_help_method(Func &&func)
{
	return register_method<registration>(".extension", ".view_help", std::forward<Func>(func));
}

} //namespace extension

} //namespace gts


#endif //GTS_DETAIL_REGISTRATION_H
