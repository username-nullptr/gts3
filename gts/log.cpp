
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

#include "log.h"
#include "private/app_info.h"
#include "gts/algorithm.h"
#include "gts/rw_mutex.h"
#include "gts/chrono.h"

#include <fmt/chrono.h>
#include <cppfilesystem>
#include <iostream>
#include <cstdio>
#include <thread>
#include <list>

namespace dt = std::chrono;

#ifdef _WINDOWS

# include <Windows.h>

#ifdef CORE_USE_STD_FS
namespace std::filesystem
#elif GTS_CPLUSPLUS >= 201703L
namespace core::filesystem
#else //cpp17
namespace core { namespace filesystem
#endif //cpp17
{

static time_t create_time(const std::string &/*file*/)
{
# error The windows has not yet been implemented.
	return 0;
}

}
#if GTS_CPLUSPLUS < 201703L
}
#endif //cpp17
// namespace xxx::filesystem

#else //other os

# include <sys/stat.h>

#ifdef CORE_USE_STD_FS
namespace std::filesystem
#elif GTS_CPLUSPLUS >= 201703L
namespace core::filesystem
#else //cpp17
namespace core { namespace filesystem
#endif //cpp17
{

static time_t create_time(const std::string &file)
{
	struct stat buf = {0};
	stat(file.c_str(), &buf);
	return buf.st_ctime;
}

}
#if GTS_CPLUSPLUS < 201703L
}
#endif //cpp17
// namespace xxx::filesystem

#endif //os

/*---------------------------------------------------------------------------------------------------------------*/

GTS_NAMESPACE_BEGIN

class GTS_DECL_HIDDEN logger_impl
{
public:
	logger_impl(const char *file, const char *func, int line);

public:
	log_buffer::context m_runtime_context;
};

logger_impl::logger_impl(const char *file, const char *func, int line)
{
	m_runtime_context.time = dt::time_point_cast<dt::milliseconds>(dt::system_clock::now());
	m_runtime_context.file = file;
	m_runtime_context.func = func;
	m_runtime_context.line = line;
}

static logger::context g_context;

static std::atomic_bool g_header_breaks_aline {false};

static rw_mutex g_context_rwlock;

/*---------------------------------------------------------------------------------------------------------------*/

logger::logger(const char *file, const char *func, int line) :
	m_impl(new logger_impl(file, func, line))
{

}

logger::~logger()
{
	delete m_impl;
}

void logger::set_context(const context &con)
{
	g_context_rwlock.lock();
	g_context = con;

	if( g_context.category.empty() )
		g_context.category = "default";

#ifdef _WINDOWS
	replace(g_context.dir, "/", "\\");
	replace(g_context.category, "/", "\\");
#else //other os
	str_replace(g_context.dir, "\\", "/");
	str_replace(g_context.category, "\\", "/");
#endif //os

	if( not g_context.dir.empty() )
		g_context.dir = app::absolute_path(g_context.dir);
	g_context_rwlock.unlock();
}

void logger::set_header_breaks_aline(bool enable)
{
	g_header_breaks_aline = enable;
}

logger::context logger::get_context()
{
	shared_lock locker(g_context_rwlock); (void)(locker);
	return g_context;
}

bool logger::get_header_breaks_aline()
{
	return g_header_breaks_aline;
}

#ifdef __NO_DEBUG__
# define LOG_OUTPUT(_type, _category)  return log_buffer(log_buffer::_type, _category)
#else //DEBUG
#define LOG_OUTPUT(_type, _category) \
({ \
	log_buffer o(log_buffer::_type); \
	o.m_data->context = m_impl->m_runtime_context; \
	o.m_data->context.category = std::move(_category); \
	return o; \
})
#endif //__NO_DEBUG__

log_buffer logger::debug()
{
	LOG_OUTPUT(debug, "");
}

log_buffer logger::info()
{
	LOG_OUTPUT(info, "");
}

log_buffer logger::warning()
{
	LOG_OUTPUT(warning, "");
}

log_buffer logger::error()
{
	LOG_OUTPUT(error, "");
}

log_buffer logger::cdebug(std::string category)
{
	LOG_OUTPUT(debug, category);
}

log_buffer logger::cinfo(std::string category)
{
	LOG_OUTPUT(info, category);
}

log_buffer logger::cwarning(std::string category)
{
	LOG_OUTPUT(warning, category);
}

log_buffer logger::cerror(std::string category)
{
	LOG_OUTPUT(error, category);
}

static void _output(log_buffer::type type, const logger::context &context,
					const log_buffer::context &runtime_context, const std::string &msg, bool async = false);

void logger::_fatal(const std::string &msg, std::string category)
{
	g_context_rwlock.lock_shared();
	auto context = g_context;
	g_context_rwlock.unlock();

	m_impl->m_runtime_context.category = std::move(category);
	_output(log_buffer::fetal, context, m_impl->m_runtime_context, msg);
	abort();
}

/*---------------------------------------------------------------------------------------------------------------*/

class GTS_DECL_HIDDEN task_thread
{
	GTS_DISABLE_COPY_MOVE(task_thread)

public:
	static task_thread &instance()
	{
		if( g_instance == nullptr )
			g_instance = new task_thread();
		return *g_instance;
	}

public:
	using duration = std::chrono::milliseconds;
	void wait(const duration &ms)
	{
		std::unique_lock<std::mutex> locker(m_wtif_mutex);
		m_wtif_condition.wait_for(locker, ms, [this]()->bool{
			return not m_wait_flag;
		});
	}

	static void reload()
	{
		if( g_instance )
			delete g_instance;
		g_instance = new task_thread();
	}

private:
	task_thread()
	{
		m_thread = std::thread([this]
		{
			while( not m_stop_flag )
			{
				std::unique_lock<std::mutex> locker(m_mutex);
				while( m_message_qeueue.empty() )
				{
					m_wait_flag = false;
					m_wtif_condition.notify_all();

					m_condition.wait(locker);
					if( m_stop_flag )
						return shutdown();
					m_wait_flag = true;
				}
				auto _node = m_message_qeueue.front();
				m_message_qeueue.pop_front();

				locker.unlock();
				_output(_node->type, *_node->context, _node->runtime_context, _node->msg, true);
			}
			m_mutex.lock();
			shutdown();
		});
	}

public:
	~task_thread()
	{
		m_stop_flag = true;
		m_condition.notify_one();
		try {
			if( m_thread.joinable() )
				m_thread.join();
		}
		catch(...) {}
	}

private:
	using context_ptr = std::shared_ptr<logger::context>;
	struct node
	{
		log_buffer::type type;
		context_ptr context;
		log_buffer::context runtime_context;
		std::string msg;

		node(log_buffer::type type, context_ptr context, log_buffer::context &&runtime_context, std::string &&msg) :
			type(type), context(std::move(context)), runtime_context(runtime_context), msg(msg) {}
	};

private:
	using node_ptr = std::shared_ptr<node>;
	std::list<node_ptr> m_message_qeueue;

public:
	void produce(log_buffer::type type, const context_ptr &context, log_buffer::context &&runtime_context, std::string &&msg)
	{
		auto _node = std::make_shared<node>(type, context, std::move(runtime_context), std::move(msg));

		m_mutex.lock();
		m_message_qeueue.emplace_back(_node);

		m_mutex.unlock();
		m_condition.notify_one();
	}

private:
	void shutdown()
	{
		std::list<node_ptr> list;
		m_message_qeueue.swap(list);
		m_mutex.unlock();

		for(auto &_node : list)
			_output(_node->type, *_node->context, _node->runtime_context, _node->msg);
	}

private:
	static task_thread *g_instance;
	std::thread m_thread;

	std::atomic_bool m_stop_flag {false};
	std::condition_variable m_condition;
	std::mutex m_mutex;

	std::atomic_bool m_wait_flag {false};
	std::condition_variable m_wtif_condition;
	std::mutex m_wtif_mutex;
};
task_thread *task_thread::g_instance = nullptr;

void logger::wait(const duration &ms)
{
	task_thread::instance().wait(ms);
}

void logger::reload()
{
	task_thread::reload();
}

GTS_DECL_HIDDEN void message_handler(log_buffer::type type, log_buffer::context &&runtime_context, std::string &&msg)
{
	g_context_rwlock.lock_shared();
	auto context = std::make_shared<logger::context>(g_context);
	g_context_rwlock.unlock();

	if( context->async and type != log_buffer::fetal )
		task_thread::instance().produce(type, context, std::move(runtime_context), std::move(msg));
	else
		_output(type, *context, runtime_context, msg);
}

static struct GTS_DECL_HIDDEN curr_log_file
{
	std::map<std::string, std::string> name_map;
	log_buffer::otime last;

	void save_last_name(const std::string &category)
	{
		auto &name = name_map[category];
		if( name.empty() )
			return ;
		auto old_name = name;
		name.erase(name.size() - 5);
		name += fmt::format("{:%H:%M:%S}", last);
		std::rename(old_name.c_str(), name.c_str());
	}
}
g_curr_log_file;

static bool open_log_output_device
(const std::string &category, const logger::context &context, const log_buffer::otime &time, std::size_t log_size, FILE *std_dev, FILE **fp);

static void _output(log_buffer::type type, const logger::context &context,
					const log_buffer::context &runtime_context, const std::string &msg, bool async)
{
	std::string log_text;
	FILE *fp = nullptr;
	bool need_close = false;
	auto &category = runtime_context.category.empty()? context.category == "default" ? "" : context.category : runtime_context.category;

#define SET_LOG(level, type, stdfp) \
({ \
	if( context.mask < level ) \
		return ; \
	log_text = fmt::format("[{}::" type "]-[{:%Y-%m-%d %H:%M:%S}]-[{}:{}]:{}", \
							category, runtime_context.time, runtime_context.file, runtime_context.line, \
							g_header_breaks_aline? "\n" : " ") + msg; \
	need_close = open_log_output_device(category, context, runtime_context.time, log_text.size(), stdfp, &fp); \
})

	static std::mutex g_file_mutex;
	switch( type )
	{
		case log_buffer::debug  : SET_LOG(4, "Debug"  , context.no_stdout ? stderr : stdout); break;
		case log_buffer::info   : SET_LOG(3, "Info"   , context.no_stdout ? stderr : stdout); break;
		case log_buffer::warning: SET_LOG(2, "Warning", stderr); break;
		case log_buffer::error  : SET_LOG(1, "Error"  , stderr); break;

		case log_buffer::fetal:
			SET_LOG(0, "Fetal", stderr);
			log_text += "\n\n";
			g_file_mutex.lock();

			std::fwrite(log_text.c_str(), 1, log_text.size(), stderr);
			std::fflush(stderr);

			if( fp != stderr )
			{
				std::fwrite(log_text.c_str(), 1, log_text.size(), fp);
				std::fflush(fp);
			}
			abort();

		default:
			std::cerr << "*** Error: Log: _output: Invalid message type" << std::endl;
			abort();
	}
	log_text += "\n\n";

	std::size_t res = 0;
	if( not async )
	{
		g_file_mutex.lock();
		res = std::fwrite(log_text.c_str(), 1, log_text.size(), fp);
		std::fflush(fp);
		g_file_mutex.unlock();
	}
	else
	{
		res = std::fwrite(log_text.c_str(), 1, log_text.size(), fp);
		std::fflush(fp);
	}
	if( res < log_text.size() and not context.dir.empty() )
		std::fprintf(stderr, "*** Error: Log: _output: The log file write error: %s.\n", strerror(errno));
	if( need_close )
		std::fclose(fp);
}

static void size_check(const logger::context &context, const std::string &dir_name, std::size_t log_size);

static bool open_log_output_device
(const std::string &category, const logger::context &context, const log_buffer::otime &time, std::size_t log_size, FILE *std_dev, FILE **fp)
{
	if( context.dir.empty() )
	{
		*fp = std_dev;
		return false;
	}
	auto &curr_file_name = g_curr_log_file.name_map[category];
	if( dt::time_point_cast<dt::days>(time) > g_curr_log_file.last )
	{
		g_curr_log_file.save_last_name(category);
		curr_file_name = "";
	}
	std::string dir_name = context.dir;
	if( not str_ends_with(dir_name, "/") )
		dir_name += "/";
	if( context.time_category )
	{
		dir_name += fmt::format("{:%Y-%m-%d}", time) + "/" + category;
		if( not str_ends_with(dir_name, "/") )
			dir_name += "/";
	}
	else
	{
		dir_name += category;
		if( not str_ends_with(dir_name, "/") )
			dir_name += "/";
		dir_name += fmt::format("{:%Y-%m-%d}", time) + "/";
	}
	std::error_code error;
	if( not fs::exists(dir_name) and not fs::create_directories(dir_name, error) )
	{
		std::cerr << "*** Error: Log: open_log_output_device: Log directory make failed: " << error.message() << std::endl;
		*fp = std_dev;
		return false;
	}
	else if( not fs::is_directory(dir_name) )
	{
		std::cerr << "*** Error: Log: open_log_output_device: '" << dir_name << "' exists, but it is not a directory." << std::endl;
		return false;
	}
	else if( curr_file_name.empty() )
	{
		try {
			for(auto &entry : fs::directory_iterator(dir_name))
			{
				auto file_name = entry.path().string();
				if( str_ends_with(file_name, "(now)") )
					curr_file_name = file_name;
			}
		}
		catch(...)
		{
			std::cerr << "*** Error: Log: open_log_output_device: Unable to traverse directory '" << dir_name << "'." << std::endl;
			return false;
		}

		if( curr_file_name.empty() )
			curr_file_name = dir_name + fmt::format("{:%H:%M:%S}-(now)", time);
	}
	else if( fs::file_size(curr_file_name) + log_size > context.max_size_one_file )
	{
		g_curr_log_file.save_last_name(category);
		curr_file_name = dir_name + fmt::format("{:%H:%M:%S}-(now)", time);
	}
	size_check(context, dir_name, log_size);
	g_curr_log_file.last = time;

	*fp = fopen(curr_file_name.c_str(), "a");
	if( *fp == nullptr )
	{
		fprintf(stderr, "*** Error: Log: openLogOutputDevice: The log file '%s' open failed: %s.\n",
				curr_file_name.c_str(), strerror(errno));
		*fp = std_dev;
		return false;
	}
	return true;
}

static void list_push_back(std::list<fs::directory_entry> &list, const std::string &dir);
static void remove_if_too_big(const std::list<fs::directory_entry> &list, std::size_t max_size, std::size_t log_size);

static void size_check(const logger::context &context, const std::string &dir_name, std::size_t log_size)
{
	std::list<fs::directory_entry> file_info_list;

	list_push_back(file_info_list, dir_name);
	remove_if_too_big(file_info_list, context.max_size_one_day, log_size);

	std::list<fs::directory_entry> dir_info_list;
	try {
		for(auto &entry : fs::directory_iterator(dir_name))
		{
			if( entry.is_directory() )
				dir_info_list.emplace_back(entry);
		}
	}
	catch(...)
	{
		std::cerr << "*** Error: Log: open_log_output_device: Unable to traverse directory '" << dir_name << "'." << std::endl;
		return ;
	}
	dir_info_list.sort([](const fs::directory_entry &info0, const fs::directory_entry &info1) {
		return fs::create_time(info0.path()) < fs::create_time(info1.path());
	});

	file_info_list.clear();
	for(auto &info : dir_info_list)
		list_push_back(file_info_list, info.path());
	remove_if_too_big(file_info_list, context.max_size, log_size);

	if( dir_info_list.size() == 1 )
		return ;
	for(auto &info : dir_info_list)
	{
		if( not fs::is_directory(info.path()) )
			continue;

		fs::directory_iterator dir_iter;
		try {
			dir_iter = fs::directory_iterator(info.path());
		}
		catch(...) {
			continue;
		}
		if( dir_iter == fs::end(dir_iter) )
			fs::remove_all(info.path());
		else
			break;
	}
}

static void list_push_back(std::list<fs::directory_entry> &list, const std::string &dir)
{
	try {
		for(auto &entry : fs::directory_iterator(dir))
		{
			if( not entry.is_directory() )
				list.emplace_back(entry);
		}
	}
	catch(...)
	{
		std::cerr << "*** Error: Log: open_log_output_device: Unable to traverse directory '" << dir << "'." << std::endl;
		return ;
	}
	list.sort([](const fs::directory_entry &info0, const fs::directory_entry &info1) {
		return fs::create_time(info0.path()) < fs::create_time(info1.path());
	});
}

static void remove_if_too_big(const std::list<fs::directory_entry> &list, std::size_t max_size, std::size_t log_size)
{
	std::size_t size = 0;
	for(auto &info : list)
		size += fs::file_size(info.path());

	if( size + log_size <= max_size )
		return ;

	for(auto &info : list)
	{
		size -= fs::file_size(info.path());
		fs::remove(info.path());

		if( size + log_size <= max_size )
			break;
	}
}

GTS_NAMESPACE_END
