//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

/*
   Portions from http://www.adp-gmbh.ch/cpp/common/base64.html
   Copyright notice:
   base64.cpp and base64.h
   Copyright (C) 2004-2008 Rene Nyffenegger
   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
   1. The origin of this source code must not be misrepresented; you must not
	  claim that you wrote the original source code. If you use this source code
	  in a product, an acknowledgment in the product documentation would be
	  appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
	  misrepresented as being the original source code.
   3. This notice may not be removed or altered from any source distribution.
   Rene Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#include "stdafx.h"
#include "Base64.h"
#include <cctype>
#include <string>
#include <utility>

namespace base64 {

	char const* get_alphabet()
	{
		static char constexpr tab[] = {
			"ABCDEFGHIJKLMNOP"
			"QRSTUVWXYZabcdef"
			"ghijklmnopqrstuv"
			"wxyz0123456789+/"
		};
		return &tab[0];
	}

	/** Encode a series of octets as a padded, base64 string.
		The resulting string will not be null terminated.
		@par Requires
		The memory pointed to by `out` points to valid memory
		of at least `encoded_size(len)` bytes.
		@return The number of characters written to `out`. This
		will exclude any null termination.
	*/
	std::size_t encode(void* dest, void const* src, std::size_t len)
	{
		char* out = static_cast<char*>(dest);
		char const* in = static_cast<char const*>(src);
		auto const tab = base64::get_alphabet();

		for (auto n = len / 3; n--;)
		{
			*out++ = tab[(in[0] & 0xfc) >> 2];
			*out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
			*out++ = tab[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)];
			*out++ = tab[in[2] & 0x3f];
			in += 3;
		}

		switch (len % 3)
		{
		case 2:
			*out++ = tab[(in[0] & 0xfc) >> 2];
			*out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
			*out++ = tab[(in[1] & 0x0f) << 2];
			*out++ = '=';
			break;

		case 1:
			*out++ = tab[(in[0] & 0xfc) >> 2];
			*out++ = tab[((in[0] & 0x03) << 4)];
			*out++ = '=';
			*out++ = '=';
			break;

		case 0:
			break;
		}

		return out - static_cast<char*>(dest);
	}

} // base64
