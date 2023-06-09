#include "session.h"
#include "service/task.h"

using namespace std::chrono;

namespace gts { namespace web
{

basic_session::~basic_session() {}

long session_config::idle_time_tv = 3000;

long session_config::max_idle_time = 60000;

long session_config::max_count = 1024;

std::atomic_long session_config::counter {0};

std::set<basic_session*> session_config::timeout_set;

void session_config::init()
{
	idle_time_tv = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_IDLE_TIME_TV, idle_time_tv);
	max_idle_time = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_MAX_IDLE_TIME, max_idle_time);
	max_count = settings::global_instance().read<long>(SINI_GROUP_WEB, SINI_WEB_MAX_SESSION_COUNT, max_count);

	if( idle_time_tv < 1 )
	{
		log_warning("Config: idle time threshold value setting error.");
		idle_time_tv = 1;
	}
	if( max_idle_time <= idle_time_tv )
	{
		log_warning("Config: max idle time setting error.");
		max_idle_time = idle_time_tv + 1;
	}
	if( max_count < 1 )
	{
		log_warning("Config: max session count setting error.");
		max_count = 1;
	}

	log_debug("Web: idle time threshold value: {}", idle_time_tv);
	log_debug("Web: max idle time: {}", max_idle_time);
	log_debug("Web: max connetion count: {}", max_count);
	task_config::init();
}

void session_config::exit()
{
	task_config::exit();
}

std::string session_config::view_status()
{
	auto result = fmt::format("web plugin:\n"
							  "  session count: {} / {}\n", counter, max_count);
	auto tmp = task_config::view_status();

	if( not tmp.empty() )
	{
		if( tmp[0] != '\n' )
			tmp = "\n" + tmp;
		result += tmp;
	}
	return result;
}

}} //namespace gts::web
