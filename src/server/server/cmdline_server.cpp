#include "cmdline_server.h"
#include "cmdline/interaction.h"
#include "gts_algorithm.h"
#include "tcp_server.h"
#include "global.h"
#include "log.h"

#ifdef _WINDOWS

# include <windows.h>
# define _BUF_SIZE  8192

#elif defined(__unix__)

# include <unistd.h>
# define _BUF_SIZE  8192

#else //other os

# include <xxx>
# define _BUF_SIZE  8192

#endif //os

namespace gts
{

cmdline_server::cmdline_server(tcp_server &_tcp_server) :
	m_interaction(new cmdline::interaction()),
	m_tcp_server(_tcp_server),
	m_buf(new char[_BUF_SIZE])
{
	if( m_interaction->open() == false )
		log_fatal("cmd line iteraction open failed.");
}

cmdline_server::~cmdline_server()
{
	stop();
	delete m_interaction;
	delete[] m_buf;
}

static std::string view_pid();

void cmdline_server::start()
{
	m_interaction->async_read(m_buf, _BUF_SIZE, [this](int size)
	{
		if( size == 0 )
			return ;

		else if( size < 0 )
			log_fatal("cmd line iteraction read error.");

		std::string str(m_buf, size);

		if( str == CCMD_VSS )
			str = view_pid() + m_tcp_server.view_status();

		else if( starts_with(str, CCMD_STSS) )
		{
			auto pos = str.find("\r\n");
			str = str.substr(pos + 2);
//            for(auto &name : string_split(str, ','))
//                gts_sm.start(trimmed(name));
			str = "accepted";
		}
		else if( starts_with(str, CCMD_SPSS) )
		{
			int pos = str.find("\r\n");
			str = str.substr(pos + 2);
//            for(auto &name : string_split(str, ','))
//                gts_sm.stop(trimmed(name));
			str = "accepted";
		}

		else if( str == CCMD_STSSA )
		{
//            gts_sm.start_all();
			str = "accepted";
		}

		else if( str == CCMD_SPSSA )
		{
//            gts_sm.stop_all();
			str = "accepted";
		}

		else if( str == CCMD_VASS )
//            str = gts_sm.view_all_server();
			str = "unrealized";

		else if( str == CCMD_VRSS )
//            str = gts_sm.view_running_server();
			str = "unrealized";

		else return start();

		int i = str.size() - 1;
		for(; i>=0; i--)
		{
			if( str[i] != '\n' )
				break;
		}
		str = str.substr(0, i + 1);

		if( not str.empty() )
			m_interaction->write(str.c_str(), str.size());

		start();
	});
}

void cmdline_server::stop()
{
	m_interaction->close();
}

static std::string view_pid()
{
#ifdef _WINDOWS

#elif defined(__unix__)

	return fmt::format("pid: {}\n", getpid());

#else //other os

#endif //os
}

} //namespace gts
