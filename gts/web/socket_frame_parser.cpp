
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

#include "socket_frame_parser.h"
#include "detail/socket_frame_impl.h"
#include <gts/byte_order.h>
#include <gts/log.h>

GTS_WEB_NAMESPACE_BEGIN

enum class status
{
	read_header,
	read_payload_length,
	read_mask,
	read_payload,
	dispatch_result
};

using close_code = socket_protocol::close_code;
using op_code = socket_protocol::op_code;

class GTS_DECL_HIDDEN socket_frame_parser_impl
{
	GTS_DISABLE_COPY_MOVE(socket_frame_parser_impl)

public:
	socket_frame_parser_impl() :
		m_cache(new socket_frame())
	{

	}
	~socket_frame_parser_impl() {
		delete m_cache;
	}

public:
	GTS_CXX_NODISCARD("") bool read_header()
	{
		if( m_abuf.size() < 2 )
			return false;

		// FIN, RSV1-3, Opcode
		auto &header = m_abuf;
		m_cache->m_impl->m_final_frame = (header[0] & 0x80) != 0;

		m_cache->m_impl->m_rsv1 = (header[0] & 0x40);
		m_cache->m_impl->m_rsv2 = (header[0] & 0x20);
		m_cache->m_impl->m_rsv3 = (header[0] & 0x10);

		m_cache->m_impl->m_op_code = static_cast<op_code>(header[0] & 0x0F);

		// Mask
		// Use zero as mask value to mean there's no mask to read.
		// When the mask value is read, it over-writes this non-zero value.
		m_cache->m_impl->m_mask = header[1] & 0x80;

		// PayloadLength
		m_cache->m_impl->m_length = (header[1] & 0x7F);
		m_abuf.erase(0,2);

		if( not check_validity() )
			return false;

		switch( m_cache->length() )
		{
			case 126:
			case 127:
				m_status = status::read_payload_length;
				break;
			default:
				m_status = m_cache->has_mask() ? status::read_mask : status::read_payload;
		}
		return true;
	}

	GTS_CXX_NODISCARD("") bool read_payload_length()
	{
		if( m_cache->length() == 126 )
		{
			if( m_abuf.size() < 2 )
				return false;

			m_cache->m_impl->m_length = ntoh(*reinterpret_cast<const uint16_t*>(m_abuf.c_str()));
			m_abuf.erase(0,2);

			if( m_cache->length() < 126 )
			{
				gts_log_info("gts::web::socket_frame_parser: Lengths smaller than 126 must be expressed as one byte.");
				return false;
			}
		}
		else if( m_cache->length() == 127 )
		{
			if( m_abuf.size() < 8 )
				return false;

			m_cache->m_impl->m_length = ntoh(*reinterpret_cast<const uint64_t*>(m_abuf.c_str()));
			m_abuf.erase(0,8);

			if( m_cache->length() & (1ULL << 63) )
			{
				gts_log_info("gts::web::socket_frame_parser: Highest bit of payload length is not 0.");
				return false;
			}
			else if( m_cache->length() <= 0xFFFFu )
			{
				gts_log_info("gts::web::socket_frame_parser: Lengths smaller than 65536 (2^16) must be expressed as 2 bytes.");
				return false;
			}
		}
		m_status = m_cache->has_mask() ? status::read_mask : status::read_payload;
		return true;
	}

	GTS_CXX_NODISCARD("") bool read_mask()
	{

	}

	GTS_CXX_NODISCARD("") bool read_payload()
	{

	}

private:
	GTS_CXX_NODISCARD("") bool check_validity() const
	{
		if( m_cache->rsv1() or m_cache->rsv2() or m_cache->rsv3() )
			gts_log_info("gts::web::socket_frame_parser: Rsv field is non-zero.");

		else if( socket_protocol::is_op_code_reserved(m_op_code) )
			gts_log_info("gts::web::socket_frame_parser: Used reserved opcode.");

		else if( m_cache->is_control_frame() )
		{
			if( m_cache->length() > 125 )
				gts_log_info("gts::web::socket_frame_parser: Control frame is larger than 125 bytes.");

			else if( not m_cache->is_final_frame() )
				gts_log_info("gts::web::socket_frame_parser: Control frames cannot be fragmented.");
			else
				m_cache->m_impl->m_valid = true;
		}
		else
			m_cache->m_impl->m_valid = true;
		return m_cache->is_valid();
	}

public:
	std::string m_abuf;
	status m_status = status::read_header;

	sp_close_code m_close_code = sp_close_code::cc_normal;
	sp_op_code m_op_code = sp_op_code::oc_reserved_c;

	socket_frame *m_cache;
};

/*-------------------------------------------------------------------------------------------------------------------*/

bool socket_frame_parser::append(const std::string &data)
{
	m_impl->m_abuf += data;
	if( is_done() )
		return true;
	else if( m_impl->m_abuf.empty() )
		return false;

	for(;;)
	{
		if( m_impl->m_status == status::read_header )
		{
			if( not m_impl->read_header() )
				break;
		}
		else if( m_impl->m_status == status::read_payload_length )
		{
			if( not m_impl->read_payload_length() )
				break;
		}
		else if( m_impl->m_status == status::read_mask )
		{
			if( not m_impl->read_mask() )
				break;
		}
		else if( m_impl->m_status == status::read_payload )
		{
			if( not m_impl->read_payload() )
				break;
		}
		else if( is_done() )
			return true;
	}
	m_impl->m_status = status::read_header;
	return false;
}

bool socket_frame_parser::is_done() const
{
	return m_impl->m_status == status::dispatch_result;
}

socket_frame socket_frame_parser::next_frame() const
{
	if( m_impl->m_cache == nullptr or not m_impl->m_cache->is_valid() )
		return {};

	auto tmp = m_impl->m_cache;
	m_impl->m_cache = new socket_frame();

	m_impl->m_status = status::read_header;
	return std::move(*tmp);
}

void socket_frame_parser::clear()
{

}

void socket_frame_parser::set_max_allowed_frame_size(std::size_t size)
{

}

std::size_t socket_frame_parser::max_allowed_frame_size() const
{
	return 0;
}

std::size_t socket_frame_parser::max_frame_size()
{
	return 0;
}

GTS_WEB_NAMESPACE_END
