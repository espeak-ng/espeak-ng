/*
 * Copyright (C) 2005 to 2007 by Jonathan Duddington
 * email: jonsd@users.sourceforge.net
 * Copyright (C) 2013-2015 Reece H. Dunn
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

#ifndef ESPEAK_NG_SPEECH_H
#define ESPEAK_NG_SPEECH_H

#include <endian.h>               // for BYTE_ORDER, BIG_ENDIAN
#include <espeak-ng/espeak_ng.h>

#if defined(__has_feature)
#  if __has_feature(memory_sanitizer)
#    include <sanitizer/msan_interface.h>
#    define MAKE_MEM_UNDEFINED(addr, len) __msan_unpoison(addr, len)
#  endif
#endif

#ifndef MAKE_MEM_UNDEFINED
#  if __has_include(<valgrind/memcheck.h>)
#    include <valgrind/memcheck.h>
#    define MAKE_MEM_UNDEFINED(addr, len) VALGRIND_MAKE_MEM_UNDEFINED(addr, len)
#  else
#    define MAKE_MEM_UNDEFINED(addr, len) ((void) ((void) addr, len))
#  endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN
#define ARCH_BIG
#endif

#ifdef __QNX__
#define NO_VARIADIC_MACROS
#endif

#if defined(_WIN32) || defined(_WIN64) // Windows

#define PLATFORM_WINDOWS 1
#define PATHSEP '\\'
#define NO_VARIADIC_MACROS
#include <windef.h>    // defines MAX_PATH (260, includes NUL)
#ifndef N_PATH_BUF
#  define N_PATH_BUF  MAX_PATH
#endif

#else

#define PLATFORM_POSIX 1
#define PATHSEP  '/'
#if defined(__linux__)
#  include <linux/limits.h>   // defines PATH_MAX (4096)
#else
#  include <limits.h>         // PATH_MAX on other POSIX systems
#endif
#define USE_NANOSLEEP
#define __cdecl

#ifndef N_PATH_BUF
#  ifdef PATH_MAX
#    define N_PATH_BUF  PATH_MAX
#  else
#    define N_PATH_BUF  4096
#  endif
#endif

#endif

// NAME_MAX: maximum length of a single filename component, excluding NUL.
// Defined by <linux/limits.h> and <limits.h> on POSIX systems. Windows has
// no standard equivalent; NTFS supports up to 255 UTF-16 code units.
#ifndef NAME_MAX
#  define NAME_MAX 255
#endif

// will look for espeak_data directory here, and also in user's home directory
#ifndef PATH_ESPEAK_DATA
#  if defined(_WIN32) || defined(_WIN64)
#    define PATH_ESPEAK_DATA "\\espeak-ng-data"
#  else
#    define PATH_ESPEAK_DATA "/espeak-ng-data"
#  endif
#endif

void cancel_audio(void);

extern char path_home[N_PATH_BUF];    // this is the espeak-ng-data directory

#ifdef __cplusplus
}
#endif

#endif // SPEECH_H
