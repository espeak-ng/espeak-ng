/* Compatibility shim for <stdint.h>
 *
 * Copyright (C) 2016 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#ifndef STDINT_H_COMPAT_SHIM
#define STDINT_H_COMPAT_SHIM

#ifdef _MSC_VER

#if _MSC_VER >= 1600 // Visual C++ 10 (Visual Studio 2010) and above...
#include <../include/stdint.h>
#else
	typedef signed __int8  int8_t;
	typedef signed __int16 int16_t;
	typedef signed __int32 int32_t;
	typedef signed __int64 int64_t;

	typedef unsigned __int8  uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;

#ifdef _WIN64
	typedef signed __int64 intptr_t;
#else
	typedef signed __int32 intptr_t;
#endif
#endif

#else

#pragma GCC system_header // Silence "warning: #include_next is a GCC extension"

#if __has_include(<stdint.h>) || !__has_include(<inttypes.h>)
#include_next <stdint.h>
#else
#include_next <inttypes.h>
#endif

#endif

#endif
