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
#  ifdef HAVE_VALGRIND_MEMCHECK_H
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

#define PLATFORM_WINDOWS
#define PATHSEP '\\'
#define N_PATH_HOME  230
#define NO_VARIADIC_MACROS

#else

#define PLATFORM_POSIX
#define PATHSEP  '/'
#define N_PATH_HOME  160
#define USE_NANOSLEEP
#define __cdecl

#endif

// will look for espeak_data directory here, and also in user's home directory
#ifndef PATH_ESPEAK_DATA
   #define PATH_ESPEAK_DATA  "/usr/share/espeak-ng-data"
#endif

void cancel_audio(void);

extern char path_home[N_PATH_HOME];    // this is the espeak-ng-data directory

#ifdef __cplusplus
}
#endif

#endif // SPEECH_H
