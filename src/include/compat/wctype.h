/* Compatibility shim for <wctype.h>
 *
 * Copyright (C) 2017 Reece H. Dunn
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

#ifndef WCTYPE_H_COMPAT_SHIM
#define WCTYPE_H_COMPAT_SHIM

#ifdef _MSC_VER
#if _MSC_VER >= 1900 // Visual C++ 14 (Visual Studio 2015) and above...
#include <../ucrt/wctype.h>
#else
#include <../include/wctype.h>
#endif
#else
#pragma GCC system_header // Silence "warning: #include_next is a GCC extension"
#include_next <wctype.h>
#endif

#include <ucd/ucd.h>

#define iswalnum  ucd_isalnum
#define iswalpha  ucd_isalpha
#define iswblank  ucd_isblank
#define iswcntrl  ucd_iscntrl
#define iswdigit  ucd_isdigit
#define iswgraph  ucd_isgraph
#define iswlower  ucd_islower
#define iswprint  ucd_isprint
#define iswpunct  ucd_ispunct
#define iswspace  ucd_isspace
#define iswupper  ucd_isupper
#define iswxdigit ucd_isxdigit

#endif
