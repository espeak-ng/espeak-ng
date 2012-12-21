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

enum android_wctype
{
	android_wctype_upper = 0x01,
	android_wctype_lower = 0x02,
	android_wctype_digit = 0x04,
	android_wctype_lettr = 0x08,
	android_wctype_space = 0x10,
	android_wctype_cntrl = 0x20,
	android_wctype_punct = 0x40,
	android_wctype_alpha = android_wctype_upper|android_wctype_lower|android_wctype_lettr,
	android_wctype_alnum = android_wctype_alpha|android_wctype_digit,
};

extern "C" android_wctype __android_wctype(wint_t c);

#define iswalpha(c) ((__android_wctype(c) & android_wctype_alpha) != 0)
#define iswalnum(c) ((__android_wctype(c) & android_wctype_alnum) != 0)
#define iswdigit(c) ((__android_wctype(c) & android_wctype_digit) != 0)
#define iswlower(c) ((__android_wctype(c) & android_wctype_lower) != 0)
#define iswspace(c) ((__android_wctype(c) & android_wctype_space) != 0)
#define iswupper(c) ((__android_wctype(c) & android_wctype_upper) != 0)

#endif
