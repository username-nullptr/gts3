
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

#ifndef GTS_WEB_DETAIL_SOCKET_H
#define GTS_WEB_DETAIL_SOCKET_H

GTS_WEB_NAMESPACE_BEGIN

inline socket_buffer::buffer(std::string data, buffer_type type) :
	type(type), data(std::move(data))
{

}

inline socket_buffer::buffer(void *data, std::size_t size, buffer_type type) :
	type(type), data(reinterpret_cast<const char*>(data), size)
{

}

template <typename...Args>
inline socket_ptr make_socket_ptr(Args&&...args) {
	return std::make_shared<socket>(std::forward<Args>(args)...);
}

GTS_WEB_NAMESPACE_END

GTS_NAMESPACE_BEGIN
template <typename...Args>
inline websocket_ptr make_websocket_ptr(Args&&...args) {
	return std::make_shared<websocket>(std::forward<Args>(args)...);
}
GTS_NAMESPACE_END


#endif //GTS_WEB_DETAIL_SOCKET_H
