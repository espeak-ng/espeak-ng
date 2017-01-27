/* Compatibility shim for <sys/stat.h>
 *
 * Copyright (C) 2016-2017 Reece H. Dunn
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

#ifndef SYS_STAT_H_COMPAT_SHIM
#define SYS_STAT_H_COMPAT_SHIM

#ifdef _MSC_VER

#if _MSC_VER >= 1900 // Visual C++ 14 (Visual Studio 2015) and above...
#include <../ucrt/sys/stat.h>
#else
#include <../include/sys/stat.h>
#endif

#else

#pragma GCC system_header // Silence "warning: #include_next is a GCC extension"
#include_next <sys/stat.h>

#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISFIFO
#define S_ISFIFO(mode) (((mode) & S_IFMT) == _S_IFIFO)
#endif

#endif
