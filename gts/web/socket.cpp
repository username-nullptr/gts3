
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

#include "socket.h"

namespace gts { namespace web
{

namespace sp = socket_protocol;

class GTS_DECL_HIDDEN socket_impl
{
	static constexpr std::size_t block_ints  = 16;
	static constexpr std::size_t block_bytes = block_ints << 2;

public:
	std::string SHA1(const std::string &input)
	{
		uint64_t transforms = 0;
		uint32_t digest[5]
		{
			0x67452301,
			0xefcdab89,
			0x98badcfe,
			0x10325476,
			0xc3d2e1f0
		};
		std::string buffer;
		std::istringstream is(input);
		for(;;)
		{
			char sbuf[block_bytes];
			is.read(sbuf, block_bytes - buffer.size());

			buffer.append(sbuf, (std::size_t)is.gcount());
			if( buffer.size() != block_bytes )
				break;

			uint32_t block[block_ints];
			buffer_to_block(buffer, block);

			transform(digest, block, transforms);
			buffer.clear();
		}
		uint64_t total_bits = (transforms * block_bytes + buffer.size()) * 8;
		buffer += static_cast<char>(0x80);
		size_t orig_size = buffer.size();

		while( buffer.size() < block_bytes )
			buffer += static_cast<char>(0x00);

		uint32_t block[block_ints];
		buffer_to_block(buffer, block);

		if( orig_size > block_bytes - 8 )
		{
			transform(digest, block, transforms);
			for(size_t i=0; i<block_ints-2; i++)
				block[i] = 0;
		}
		block[block_ints - 1] = static_cast<uint32_t>(total_bits >>  0);
		block[block_ints - 2] = static_cast<uint32_t>(total_bits >> 32);
		transform(digest, block, transforms);

		std::ostringstream result;
		for(size_t i=0; i<sizeof(digest)/sizeof(digest[0]); i++)
		{
			result << std::hex << std::setfill('0') << std::setw(8);
			result << digest[i];
		}
		return result.str();
	}

private:
	void buffer_to_block(const std::string &buffer, uint32_t block[block_ints])
	{
		for(size_t i=0; i<block_ints; i++)
		{
			block[i] = (buffer[4 * i + 0] & 0xff) << 24 |
					   (buffer[4 * i + 1] & 0xff) << 16 |
					   (buffer[4 * i + 2] & 0xff) <<  8 |
					   (buffer[4 * i + 3] & 0xff) <<  0 ;
		}
	}

	void transform(uint32_t digest[], uint32_t block[block_ints], uint64_t &transforms)
	{
		uint32_t a = digest[0];
		uint32_t b = digest[1];
		uint32_t c = digest[2];
		uint32_t d = digest[3];
		uint32_t e = digest[4];

		R0(block, a, b, c, d, e,  0);
		R0(block, e, a, b, c, d,  1);
		R0(block, d, e, a, b, c,  2);
		R0(block, c, d, e, a, b,  3);
		R0(block, b, c, d, e, a,  4);
		R0(block, a, b, c, d, e,  5);
		R0(block, e, a, b, c, d,  6);
		R0(block, d, e, a, b, c,  7);
		R0(block, c, d, e, a, b,  8);
		R0(block, b, c, d, e, a,  9);
		R0(block, a, b, c, d, e, 10);
		R0(block, e, a, b, c, d, 11);
		R0(block, d, e, a, b, c, 12);
		R0(block, c, d, e, a, b, 13);
		R0(block, b, c, d, e, a, 14);
		R0(block, a, b, c, d, e, 15);
		R1(block, e, a, b, c, d,  0);
		R1(block, d, e, a, b, c,  1);
		R1(block, c, d, e, a, b,  2);
		R1(block, b, c, d, e, a,  3);
		R2(block, a, b, c, d, e,  4);
		R2(block, e, a, b, c, d,  5);
		R2(block, d, e, a, b, c,  6);
		R2(block, c, d, e, a, b,  7);
		R2(block, b, c, d, e, a,  8);
		R2(block, a, b, c, d, e,  9);
		R2(block, e, a, b, c, d, 10);
		R2(block, d, e, a, b, c, 11);
		R2(block, c, d, e, a, b, 12);
		R2(block, b, c, d, e, a, 13);
		R2(block, a, b, c, d, e, 14);
		R2(block, e, a, b, c, d, 15);
		R2(block, d, e, a, b, c,  0);
		R2(block, c, d, e, a, b,  1);
		R2(block, b, c, d, e, a,  2);
		R2(block, a, b, c, d, e,  3);
		R2(block, e, a, b, c, d,  4);
		R2(block, d, e, a, b, c,  5);
		R2(block, c, d, e, a, b,  6);
		R2(block, b, c, d, e, a,  7);
		R3(block, a, b, c, d, e,  8);
		R3(block, e, a, b, c, d,  9);
		R3(block, d, e, a, b, c, 10);
		R3(block, c, d, e, a, b, 11);
		R3(block, b, c, d, e, a, 12);
		R3(block, a, b, c, d, e, 13);
		R3(block, e, a, b, c, d, 14);
		R3(block, d, e, a, b, c, 15);
		R3(block, c, d, e, a, b,  0);
		R3(block, b, c, d, e, a,  1);
		R3(block, a, b, c, d, e,  2);
		R3(block, e, a, b, c, d,  3);
		R3(block, d, e, a, b, c,  4);
		R3(block, c, d, e, a, b,  5);
		R3(block, b, c, d, e, a,  6);
		R3(block, a, b, c, d, e,  7);
		R3(block, e, a, b, c, d,  8);
		R3(block, d, e, a, b, c,  9);
		R3(block, c, d, e, a, b, 10);
		R3(block, b, c, d, e, a, 11);
		R4(block, a, b, c, d, e, 12);
		R4(block, e, a, b, c, d, 13);
		R4(block, d, e, a, b, c, 14);
		R4(block, c, d, e, a, b, 15);
		R4(block, b, c, d, e, a,  0);
		R4(block, a, b, c, d, e,  1);
		R4(block, e, a, b, c, d,  2);
		R4(block, d, e, a, b, c,  3);
		R4(block, c, d, e, a, b,  4);
		R4(block, b, c, d, e, a,  5);
		R4(block, a, b, c, d, e,  6);
		R4(block, e, a, b, c, d,  7);
		R4(block, d, e, a, b, c,  8);
		R4(block, c, d, e, a, b,  9);
		R4(block, b, c, d, e, a, 10);
		R4(block, a, b, c, d, e, 11);
		R4(block, e, a, b, c, d, 12);
		R4(block, d, e, a, b, c, 13);
		R4(block, c, d, e, a, b, 14);
		R4(block, b, c, d, e, a, 15);

		digest[0] += a;
		digest[1] += b;
		digest[2] += c;
		digest[3] += d;
		digest[4] += e;

		transforms++;
	}

	void R0(const uint32_t block[block_ints], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
	{
		z += ( ( w & (x ^ y) ) ^ y ) + block[i] + 0X5A827999 + rol(v, 5);
		w = rol(w, 30);
	}

	void R1(uint32_t block[block_ints], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
	{
		block[i] = blk(block, i);
		z += ( ( w & (x ^ y) ) ^ y ) + block[i] + 0X5A827999 + rol(v, 5);
		w = rol(w, 30);
	}

	void R2(uint32_t block[block_ints], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
	{
		block[i] = blk(block, i);
		z += (w ^ x ^ y) + block[i] + 0X6ED9EBA1 + rol(v, 5);
		w = rol(w, 30);
	}

	void R3(uint32_t block[block_ints], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
	{
		block[i] = blk(block, i);
		z += ( ( (w | x) & y ) | (w & x) ) + block[i] + 0X8F1BBCDC + rol(v, 5);
		w = rol(w, 30);
	}

	void R4(uint32_t block[block_ints], const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
	{
		block[i] = blk(block, i);
		z += (w ^ x ^ y) + block[i] + 0XCA62C1D6 + rol(v, 5);
		w = rol(w, 30);
	}

	inline uint32_t rol(const uint32_t value, const size_t bits) {
		return (value << bits) | (value >> (32 - bits));
	}

	inline uint32_t blk(const uint32_t block[block_ints], const size_t i) {
		return rol(block[(i + 13) & 15] ^ block[(i + 8) & 15] ^ block[(i + 2) & 15] ^ block[i], 1);
	}

	void reset(uint32_t digest[], std::string &buffer, uint64_t &transforms)
	{
		buffer = "";
		transforms = 0;
	}

public:
	tcp_socket_ptr m_socket;
	sp::version m_version = sp::version_unknown;
};

socket::socket(const http::request &req, http::response &resp, const std::string &sec_websocket_protocol) noexcept(false) :
	m_impl(new socket_impl())
{
	auto &headers = req.headers();
	auto it = headers.find(http::header::upgrade);

	if( it == headers.end() or str_to_lower(it->second) != "websocket" )
		throw exception("gts::websocket: Protocol upgrade is not supported.");

	it = headers.find("Sec-WebSocket-Key");
	if( it == headers.end() )
	{
		resp.set_status(http::hs_forbidden).write("'Sec-WebSocket-Key' not found.");
		throw exception("gts::websocket: http header 'Sec-WebSocket-Key' not found.");
	}
	auto &sw_key = it->second;
	auto swp_list = str_split(req.header_or("Sec-WebSocket-Protocol"), ",");

	for(auto &sswp : swp_list)
		sswp = str_trimmed(sswp);

	if( sec_websocket_protocol.empty() )
		resp.set_header("Sec-WebSocket-Protocol", swp_list.empty() ? "" : swp_list.front());
	else
	{
		auto it = std::find(swp_list.begin(), swp_list.end(), sec_websocket_protocol);
		if( it == swp_list.end() )
		{
			resp.set_status(http::hs_forbidden).write("'Sec-WebSocket-Protocol' not supported.");
			throw exception("gts::websocket: 'Sec-WebSocket-Protocol' not supported.");
		}
	}
	resp.set_header("Sec-WebSocket-Accept", m_impl->SHA1(sw_key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"))
		.set_status(http::hs_switching_protocols)
		.write();
	m_impl->m_version = req.header_or("Sec-WebSocket-Version", sp::version_unknown);
	m_impl->m_socket = resp.take();
}

socket::~socket() noexcept
{
	delete m_impl;
}

sp::version socket::version() const
{
	return m_impl->m_version;
}

bool socket::is_open() const
{
	return m_impl->m_socket != nullptr;
}

const tcp_socket_ptr socket::native() const noexcept
{
	return m_impl->m_socket;
}

tcp_socket_ptr socket::native() noexcept
{
	return m_impl->m_socket;
}

}} //namespace gts::web
