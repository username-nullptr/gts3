
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

#ifndef GTS_SHA1_H
#define GTS_SHA1_H

#include <gts/global.h>

GTS_NAMESPACE_BEGIN

class sha1_impl;

class GTSCORE_API sha1
{
public:
	sha1(const std::string &text = "");
	sha1(const sha1 &other);
	sha1(sha1 &&other);
	~sha1();

public:
	sha1 &operator=(const sha1 &other);
	sha1 &operator=(sha1 &&other);

public:
	sha1 &append(uint8_t x);
	sha1 &append(char c);
	sha1 &append(const void *data, std::size_t size);
	sha1 &append(const std::string &text);

public:
	void operator+=(uint8_t x);
	void operator+=(char c);
	void operator+=(const std::string &text);

public:
	sha1 &finalize();
	std::string hex(bool upper_case = true) const;
	std::string base64() const;

private:
	sha1_impl *m_impl;
};

GTS_NAMESPACE_END


#endif //GTS_SHA1H
