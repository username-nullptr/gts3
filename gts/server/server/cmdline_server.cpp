
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

#include "cmdline_server.h"
#include "cmdline/interaction.h"
#include "gts/algorithm.h"
#include "gts/log.h"

#include "tcp_server.h"
#include "global.h"

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
		gts_log_fatal("cmd line iteraction open failed.");
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
			gts_log_fatal("cmd line iteraction read error.");

		std::string str(m_buf, size);
		gts_log_debug("cmdline_server: read: {}", str);

		if( str == CCMD_VSS )
			str = "\n" + view_pid() + m_tcp_server.view_status();

		else if( str_starts_with(str, CCMD_STSS) )
		{
			auto pos = str.find("\r\n");
			str = str.substr(pos + 2);
//            for(auto &name : string_split(str, ','))
//                gts_sm.start(trimmed(name));
			str = "accepted";
		}
		else if( str_starts_with(str, CCMD_SPSS) )
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
		{
			str += "\n";
			m_interaction->write(str.c_str(), str.size());
		}
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
