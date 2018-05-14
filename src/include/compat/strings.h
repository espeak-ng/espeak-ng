/* Compatibility shim for <strings.h>
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

#ifndef STRINGS_H_COMPAT_SHIM
#define STRINGS_H_COMPAT_SHIM

#ifdef _MSC_VER

#include <string.h>

#define strcasecmp stricmp

#else
#pragma GCC system_header // Silence "warning: #include_next is a GCC extension"
#include_next <strings.h>
#endif

#endif
