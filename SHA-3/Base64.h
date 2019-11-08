#pragma once
//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BASE64_H_
#define BASE64_H_

#include <cctype>
#include <utility>

namespace base64 {
	char const* get_alphabet();

	signed char const* get_inverse();

	/// Returns max chars needed to encode a base64 string
	std::size_t constexpr encoded_size(std::size_t n)
	{
		return 4 * ((n + 2) / 3);
	}

	/** Encode a series of octets as a padded, base64 string.
		The resulting string will not be null terminated.
		@par Requires
		The memory pointed to by `out` points to valid memory
		of at least `encoded_size(len)` bytes.
		@return The number of characters written to `out`. This
		will exclude any null termination.
	*/
	std::size_t encode(void* dest, void const* src, std::size_t len);

} // base64

#endif