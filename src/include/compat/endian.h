/* Compatibility shim for <endian.h>
 *
 * "License": Public Domain
 * "Original": https://gist.github.com/panzi/6856583
 *
 * I, Mathias Panzenböck, place this file hereby into the public domain. Use it
 * at your own risk for whatever you like. In case there are jurisdictions that
 * don't support putting things in the public domain you can also consider it
 * to be "dual licensed" under the BSD, MIT and Apache licenses, if you want
 * to. This code is trivial anyway. Consider it an example on how to get the
 * endian conversion functions on different platforms.
 *
 * Modifications also in the Public Domain and dual licensed under BSD, MIT and
 * Apache licenses (using the terms outlined above):
 *
 * Copyright (C) 2016 Reece H. Dunn
 */

#ifndef ENDIAN_H_COMPAT_SHIM
#define ENDIAN_H_COMPAT_SHIM
#pragma once

#if __has_include_next(<endian.h>)
#	pragma GCC system_header // Silence "warning: #include_next is a GCC extension"
#	include_next <endian.h>
#elif __has_include(<sys/endian.h>)
#	include <sys/endian.h>
#	if !defined(be16toh)
#		define be16toh(x) betoh16(x)
#	endif
#	if !defined(le16toh)
#		define le16toh(x) letoh16(x)
#	endif
#	if !defined(be32toh)
#		define be32toh(x) betoh32(x)
#	endif
#	if !defined(le32toh)
#		define le32toh(x) letoh32(x)
#	endif
#	if !defined(be64toh)
#		define be64toh(x) betoh64(x)
#	endif
#	if !defined(le64toh)
#		define le64toh(x) letoh64(x)
#	endif
#elif defined(__APPLE__)
#	include <libkern/OSByteOrder.h>

#	define htobe16(x) OSSwapHostToBigInt16(x)
#	define htole16(x) OSSwapHostToLittleInt16(x)
#	define be16toh(x) OSSwapBigToHostInt16(x)
#	define le16toh(x) OSSwapLittleToHostInt16(x)
 
#	define htobe32(x) OSSwapHostToBigInt32(x)
#	define htole32(x) OSSwapHostToLittleInt32(x)
#	define be32toh(x) OSSwapBigToHostInt32(x)
#	define le32toh(x) OSSwapLittleToHostInt32(x)
 
#	define htobe64(x) OSSwapHostToBigInt64(x)
#	define htole64(x) OSSwapHostToLittleInt64(x)
#	define be64toh(x) OSSwapBigToHostInt64(x)
#	define le64toh(x) OSSwapLittleToHostInt64(x)

#	define __BYTE_ORDER    BYTE_ORDER
#	define __BIG_ENDIAN    BIG_ENDIAN
#	define __LITTLE_ENDIAN LITTLE_ENDIAN
#	define __PDP_ENDIAN    PDP_ENDIAN
#elif defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
#	if BYTE_ORDER == LITTLE_ENDIAN

#		define htobe16(x) htons(x)
#		define htole16(x) (x)
#		define be16toh(x) ntohs(x)
#		define le16toh(x) (x)
 
#		define htobe32(x) htonl(x)
#		define htole32(x) (x)
#		define be32toh(x) ntohl(x)
#		define le32toh(x) (x)
 
#		define htobe64(x) htonll(x)
#		define htole64(x) (x)
#		define be64toh(x) ntohll(x)
#		define le64toh(x) (x)
#	elif BYTE_ORDER == BIG_ENDIAN /* that would be xbox 360 */
#		define htobe16(x) (x)
#		define htole16(x) __builtin_bswap16(x)
#		define be16toh(x) (x)
#		define le16toh(x) __builtin_bswap16(x)
 
#		define htobe32(x) (x)
#		define htole32(x) __builtin_bswap32(x)
#		define be32toh(x) (x)
#		define le32toh(x) __builtin_bswap32(x)
 
#		define htobe64(x) (x)
#		define htole64(x) __builtin_bswap64(x)
#		define be64toh(x) (x)
#		define le64toh(x) __builtin_bswap64(x)
#	else
#		error byte order not supported
#	endif

#	define __BYTE_ORDER    BYTE_ORDER
#	define __BIG_ENDIAN    BIG_ENDIAN
#	define __LITTLE_ENDIAN LITTLE_ENDIAN
#	define __PDP_ENDIAN    PDP_ENDIAN
#elif defined(__sun) && defined(__SVR4) /* Solaris */
#   include <sys/byteorder.h>

#   define htobe16(x) BE_16(x)
#   define htole16(x) LE_16(x)
#   define be16toh(x) BE_16(x)
#   define le16toh(x) LE_16(x)

#   define htobe32(x) BE_32(x)
#   define htole32(x) LE_32(x)
#   define be32toh(x) BE_32(x)
#   define le32toh(x) LE_32(x)

#   define htobe64(x) BE_64(x)
#   define htole64(x) LE_64(x)
#   define be64toh(x) BE_64(x)
#   define le64toh(x) LE_64(x)
#else
#	error platform not supported
#endif
#endif
