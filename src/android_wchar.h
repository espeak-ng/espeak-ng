/* Wide-Character Locale Compatibility API for Android.
 *
 * Copyright (C) 2012 Reece H. Dunn
 *
 * This file is part of espeak.
 *
 * espeak is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * espeak is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with espeak.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANDROID_WCHAR_H
#define ANDROID_WCHAR_H

#include <stdint.h>
#include <wchar.h>
#include <wctype.h>

#ifdef ANDROID

#include "ucd/ucd.h"

#define iswalpha(c) ucd::isalpha(c)
#define iswalnum(c) ucd::isalnum(c)
#define iswdigit(c) ucd::isdigit(c)
#define iswlower(c) ucd::islower(c)
#define iswspace(c) ucd::isspace(c)
#define iswupper(c) ucd::isupper(c)

#define towlower(c) ucd::tolower(c)
#define towupper(c) ucd::toupper(c)

#endif
#endif
