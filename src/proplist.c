/* PropList APIs.
 *
 * Copyright (C) 2017 Reece H. Dunn
 *
 * This file is part of ucd-tools.
 *
 * ucd-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ucd-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ucd-tools.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ucd/ucd.h"

static int properties_Cc(codepoint_t c)
{
	if (c >= 0x0009 && c <= 0x000D) return UCD_PROPERTY_WHITE_SPACE;
	if (c == 0x0085)                return UCD_PROPERTY_WHITE_SPACE;
	return 0;
}

static int properties_Cf(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0000:
		if (c == 0x00AD)                return UCD_PROPERTY_HYPHEN;
		break;
	case 0x0600:
		if (c == 0x061C)                return UCD_PROPERTY_BIDI_CONTROL;
		break;
	case 0x2000:
		if (c >= 0x200C && c <= 0x200D) return UCD_PROPERTY_JOIN_CONTROL;
		if (c >= 0x200E && c <= 0x200F) return UCD_PROPERTY_BIDI_CONTROL;
		if (c >= 0x202A && c <= 0x202E) return UCD_PROPERTY_BIDI_CONTROL;
		if (c >= 0x2061 && c <= 0x2064) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x2066 && c <= 0x2069) return UCD_PROPERTY_BIDI_CONTROL;
		break;
	}
	return 0;
}

static int properties_Ll(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0300:
		if (c >= 0x03D0 && c <= 0x03D2) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c == 0x03D5)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x03F0 && c <= 0x03F1) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x03F4 && c <= 0x03F5) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x2100:
		if (c >= 0x210A && c <= 0x2113) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x212F && c <= 0x2131) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x2133 && c <= 0x2134) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x213C && c <= 0x213F) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x2145 && c <= 0x2149) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x01D400:
		if (c >= 0x01D400 && c <= 0x01D454) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D456 && c <= 0x01D49C) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D4AE && c <= 0x01D4B9) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c == 0x01D4BB)                  return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D4BD && c <= 0x01D4C3) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D4C5)                  return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x01D500:
		if                  (c <= 0x01D505) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D51E && c <= 0x01D539) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D552)                  return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x01D600:
		if                  (c <= 0x01D6A5) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D6C2 && c <= 0x01D6DA) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D6DC && c <= 0x01D6FA) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D6FC)                  return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x01D700:
		if                  (c <= 0x01D714) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D716 && c <= 0x01D734) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D736 && c <= 0x01D74E) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D750 && c <= 0x01D76E) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D770 && c <= 0x01D788) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D78A && c <= 0x01D7A8) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D7AA && c <= 0x01D7C2) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D7C4 && c <= 0x01D7CB) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	}
	return 0;
}

static int properties_Lo(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x2100:
		if (c >= 0x2135 && c <= 0x2138) return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x01EE00:
		return UCD_PROPERTY_OTHER_MATH;
	}
	return 0;
}

static int properties_Lu(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0300:
		if (c >= 0x03D0 && c <= 0x03D2) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x03F4 && c <= 0x03F5) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x2100:
		if (c == 0x2102)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x2107)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x210A && c <= 0x2113) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c == 0x2115)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x2119 && c <= 0x211D) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x2124)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x2128)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x212C && c <= 0x212D) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x212F && c <= 0x2131) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x2133 && c <= 0x2134) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x213C && c <= 0x213F) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x2145 && c <= 0x2149) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x01D400:
		if (c >= 0x01D400 && c <= 0x01D454) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D456 && c <= 0x01D49C) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D49E && c <= 0x01D49F) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x01D4A2)                  return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D4A5 && c <= 0x01D4A6) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D4A9 && c <= 0x01D4AC) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D4AE && c <= 0x01D4B9) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D4C5)                  return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x01D500:
		if                  (c <= 0x01D505) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D507 && c <= 0x01D50A) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D50D && c <= 0x01D514) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D516 && c <= 0x01D51C) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D51E && c <= 0x01D539) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D53B && c <= 0x01D53E) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D540 && c <= 0x01D544) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x01D546)                  return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D54A && c <= 0x01D550) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D552)                  return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x01D600:
		if                  (c <= 0x01D6A5) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D6A8 && c <= 0x01D6C0) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x01D6DC && c <= 0x01D6FA) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	case 0x01D700:
		if (c >= 0x01D716 && c <= 0x01D734) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D750 && c <= 0x01D76E) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D78A && c <= 0x01D7A8) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		if (c >= 0x01D7C4 && c <= 0x01D7CB) return UCD_PROPERTY_OTHER_MATH; // Ll|Lu
		break;
	}
	return 0;
}

static int properties_Mn(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x2000:
		if (c >= 0x20D0 && c <= 0x20DC) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x20E1)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x20E5 && c <= 0x20E6) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x20EB && c <= 0x20EF) return UCD_PROPERTY_OTHER_MATH;
		break;
	}
	return 0;
}

static int properties_Nd(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x01D700:
		if (c >= 0x01D7CE && c <= 0x01D7FF) return UCD_PROPERTY_OTHER_MATH;
		break;
	}
	return 0;
}

static int properties_Pc(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x2000:
		if (c == 0x2040)                return UCD_PROPERTY_OTHER_MATH;
		break;
	}
	return 0;
}

static int properties_Pd(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0000:
		if (c == 0x002D)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN;
		break;
	case 0x0500:
		if (c == 0x058A)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN;
		break;
	case 0x1800:
		if (c == 0x1806)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN;
		break;
	case 0x2000:
		if (c == 0x2010)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN;
		if (c == 0x2011)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN | UCD_PROPERTY_NO_BREAK;
		break;
	case 0x2E00:
		if (c == 0x2E17)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN;
		break;
	case 0xFE00:
		if (c == 0xFE63)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN | UCD_PROPERTY_OTHER_MATH;
		break;
	case 0xFF00:
		if (c == 0xFF0D)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN;
		break;
	}
	return UCD_PROPERTY_DASH;
}

static int properties_Pe(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x2000:
		if (c == 0x207E)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x208E)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2300:
		if (c == 0x2309)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x230B)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2700:
		if (c == 0x27C6)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x27E6 && c <= 0x27EF) return UCD_PROPERTY_OTHER_MATH; // Pe|Ps
		break;
	case 0x2900:
		if (c >= 0x2983 && c <= 0x2998) return UCD_PROPERTY_OTHER_MATH; // Pe|Ps
		if (c >= 0x29D8 && c <= 0x29DB) return UCD_PROPERTY_OTHER_MATH; // Pe|Ps
		if (c == 0x29FD)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x3000:
		if (c == 0x300D)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x300F)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c >= 0x301E && c <= 0x301F) return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0xFE00:
		if (c == 0xFE42)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0xFE44)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0xFF00:
		if (c == 0xFF63)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	}
	return 0;
}

static int properties_Pf(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0000:
		if (c == 0x00BB)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0x2000:
		if (c == 0x2019)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x201D)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x203A)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	}
	return 0;
}

static int properties_Pi(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0000:
		if (c == 0x00AB)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0x2000:
		if (c == 0x2018)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c >= 0x201B && c <= 0x201C) return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x201F)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x2039)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	}
	return 0;
}

static int properties_Po(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0000:
		if (c == 0x0021)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x0022)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x0027)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x002C)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x002E)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c >= 0x003A && c <= 0x003B) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x003F)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0300:
		if (c == 0x037E)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x0387)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0500:
		if (c == 0x0589)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x05C3)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0600:
		if (c == 0x060C)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x061B)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x061F)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x06D4)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0700:
		if (c >= 0x0700 && c <= 0x070A) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x070C)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c >= 0x07F8 && c <= 0x07F9) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0800:
		if (c >= 0x0830 && c <= 0x083E) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x085E)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0900:
		if (c >= 0x0964 && c <= 0x0965) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0E00:
		if (c >= 0x0E5A && c <= 0x0E5B) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x0F00:
		if (c == 0x0F08)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x0F0C)                return UCD_PROPERTY_NO_BREAK;
		if (c >= 0x0F0D && c <= 0x0F12) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x1000:
		if (c >= 0x104A && c <= 0x104B) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x1300:
		if (c >= 0x1361 && c <= 0x1368) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x1600:
		if (c >= 0x166D && c <= 0x166E) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c >= 0x16EB && c <= 0x16ED) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x1700:
		if (c >= 0x1735 && c <= 0x1736) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c >= 0x17D4 && c <= 0x17D6) return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		if (c == 0x17DA)                return UCD_PROPERTY_TERMINAL_PUNCTUATION;
		break;
	case 0x1800:
		if (c >= 0x1802 && c <= 0x1805) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x1808 && c <= 0x1809) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x1900:
		if (c >= 0x1944 && c <= 0x1945) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x1A00:
		if (c >= 0x1AA8 && c <= 0x1AAB) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x1B00:
		if (c >= 0x1B5A && c <= 0x1B5B) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x1B5D && c <= 0x1B5F) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x1C00:
		if (c >= 0x1C3B && c <= 0x1C3F) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x1C7E && c <= 0x1C7F) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x2000:
		if (c == 0x2016)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x2032 && c <= 0x2034) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x203C && c <= 0x203D) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x2047 && c <= 0x2049) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x2053)                return UCD_PROPERTY_DASH;
		break;
	case 0x2E00:
		if (c == 0x2E2E)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x2E3C)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x2E41)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x3000:
		if (c >= 0x3001 && c <= 0x3002) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x30FB)                return UCD_PROPERTY_HYPHEN;
		break;
	case 0xA400:
		if (c >= 0xA4FE && c <= 0xA4FF) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0xA600:
		if (c >= 0xA60D && c <= 0xA60F) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0xA6F3 && c <= 0xA6F7) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0xA800:
		if (c >= 0xA876 && c <= 0xA877) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0xA8CE && c <= 0xA8CF) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0xA900:
		if (c == 0xA92F)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0xA9C7 && c <= 0xA9C9) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0xAA00:
		if (c >= 0xAA5D && c <= 0xAA5F) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xAADF)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0xAAF0 && c <= 0xAAF1) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0xAB00:
		if (c == 0xABEB)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0xFE00:
		if (c >= 0xFE50 && c <= 0xFE52) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0xFE54 && c <= 0xFE57) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xFE61)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0xFE68)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0xFF00:
		if (c == 0xFF01)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xFF02)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0xFF07)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0xFF0C)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xFF0E)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xFF3C)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0xFF65)                return UCD_PROPERTY_HYPHEN;
		if (c >= 0xFF1A && c <= 0xFF1B) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xFF1F)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xFF61)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0xFF64)                return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x10300:
		if (c == 0x01039F)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x0103D0)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x10800:
		if (c == 0x010857)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x10900:
		if (c == 0x01091F)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x10A00:
		if (c >= 0x010A56 && c <= 0x010A57) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x010AF0 && c <= 0x010AF5) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x10B00:
		if (c >= 0x010B3A && c <= 0x010B3F) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x010B99 && c <= 0x010B9C) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11000:
		if (c >= 0x011047 && c <= 0x01104D) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x0110BE && c <= 0x0110C1) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11100:
		if (c >= 0x011141 && c <= 0x011143) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x0111C5 && c <= 0x0111C6) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x0111CD)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x0111DE && c <= 0x0111DF) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11200:
		if (c >= 0x011238 && c <= 0x01123C) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x0112A9)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11400:
		if (c >= 0x01144B && c <= 0x01144D) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x01145B)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11500:
		if (c >= 0x0115C2 && c <= 0x0115C5) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c >= 0x0115C9 && c <= 0x0115D7) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11600:
		if (c >= 0x011641 && c <= 0x011642) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11700:
		if (c >= 0x01173C && c <= 0x01173E) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x11C00:
		if (c >= 0x011C41 && c <= 0x011C43) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x011C71)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x12400:
		if (c >= 0x012470 && c <= 0x012474) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x16A00:
		if (c >= 0x016A6E && c <= 0x016A6F) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x016AF5)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x16B00:
		if (c >= 0x016B37 && c <= 0x016B39) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		if (c == 0x016B44)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x1BC00:
		if (c == 0x01BC9F)                  return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	case 0x1DA00:
		if (c >= 0x01DA87 && c <= 0x01DA8A) return UCD_PROPERTY_TERMINAL_PUNCTUATION; 
		break;
	}
	return 0;
}

static int properties_Ps(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x2000:
		if (c == 0x201A)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x201E)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x207D)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x208D)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2300:
		if (c == 0x2308)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x230A)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2700:
		if (c == 0x27C5)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x27E6 && c <= 0x27EF) return UCD_PROPERTY_OTHER_MATH; // Pe|Ps
		break;
	case 0x2900:
		if (c >= 0x2983 && c <= 0x2998) return UCD_PROPERTY_OTHER_MATH; // Pe|Ps
		if (c >= 0x29D8 && c <= 0x29DB) return UCD_PROPERTY_OTHER_MATH; // Pe|Ps
		if (c == 0x29FC)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2E00:
		if (c == 0x2E42)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0x3000:
		if (c == 0x300C)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x300E)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x301D)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0xFE00:
		if (c == 0xFE41)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0xFE43)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0xFF00:
		if (c == 0xFF62)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	}
	return 0;
}

static int properties_Sk(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0000:
		if (c == 0x005E)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0xFF00:
		if (c == 0xFF3E)                return UCD_PROPERTY_OTHER_MATH;
		break;
	}
	return 0;
}

static int properties_Sm(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x2000:
		if (c == 0x207B)                return UCD_PROPERTY_DASH;
		if (c == 0x208B)                return UCD_PROPERTY_DASH;
		break;
	case 0x2200:
		if (c == 0x2212)                return UCD_PROPERTY_DASH;
		break;
	}
	return 0;
}

static int properties_So(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x2100:
		if (c == 0x2129)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x2195 && c <= 0x2199) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x219C && c <= 0x219F) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21A1 && c <= 0x21A2) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21A4 && c <= 0x21A5) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x21A7)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21A9 && c <= 0x21AD) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21B0 && c <= 0x21B1) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21B6 && c <= 0x21B7) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21BC && c <= 0x21CD) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21D0 && c <= 0x21D1) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x21D3)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21D5 && c <= 0x21DB) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x21DD)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x21E4 && c <= 0x21E5) return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2300:
		if (c >= 0x23B4 && c <= 0x23B5) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x23B7)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x23D0)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x23E2)                return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2500:
		if (c >= 0x25A0 && c <= 0x25A1) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x25AE && c <= 0x25B6) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x25BC && c <= 0x25C0) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x25C6 && c <= 0x25C7) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x25CA && c <= 0x25CB) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x25CF && c <= 0x25D3) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x25E2)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x25E4)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x25E7 && c <= 0x25EC) return UCD_PROPERTY_OTHER_MATH;
		break;
	case 0x2600:
		if (c >= 0x2605 && c <= 0x2606) return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x2640)                return UCD_PROPERTY_OTHER_MATH;
		if (c == 0x2642)                return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x2660 && c <= 0x2663) return UCD_PROPERTY_OTHER_MATH;
		if (c >= 0x266D && c <= 0x266E) return UCD_PROPERTY_OTHER_MATH;
		break;
	}
	return 0;
}

static int properties_Zs(codepoint_t c)
{
	switch (c)
	{
	case 0x00A0: // NO-BREAK SPACE
	case 0x2007: // FIGURE SPACE
	case 0x202F: // NARROW NO-BREAK SPACE
		return UCD_PROPERTY_WHITE_SPACE | UCD_PROPERTY_NO_BREAK;
	}
	return UCD_PROPERTY_WHITE_SPACE;
}

ucd_property ucd_properties(codepoint_t c, ucd_category category)
{
	switch (category)
	{
	case UCD_CATEGORY_Cc: return properties_Cc(c);
	case UCD_CATEGORY_Cf: return properties_Cf(c);
	case UCD_CATEGORY_Ll: return properties_Ll(c);
	case UCD_CATEGORY_Lo: return properties_Lo(c);
	case UCD_CATEGORY_Lu: return properties_Lu(c);
	case UCD_CATEGORY_Mn: return properties_Mn(c);
	case UCD_CATEGORY_Nd: return properties_Nd(c);
	case UCD_CATEGORY_Pc: return properties_Pc(c);
	case UCD_CATEGORY_Pd: return properties_Pd(c);
	case UCD_CATEGORY_Pe: return properties_Pe(c);
	case UCD_CATEGORY_Pf: return properties_Pf(c);
	case UCD_CATEGORY_Pi: return properties_Pi(c);
	case UCD_CATEGORY_Po: return properties_Po(c);
	case UCD_CATEGORY_Ps: return properties_Ps(c);
	case UCD_CATEGORY_Sk: return properties_Sk(c);
	case UCD_CATEGORY_Sm: return properties_Sm(c);
	case UCD_CATEGORY_So: return properties_So(c);
	case UCD_CATEGORY_Zl: return UCD_PROPERTY_WHITE_SPACE;
	case UCD_CATEGORY_Zp: return UCD_PROPERTY_WHITE_SPACE;
	case UCD_CATEGORY_Zs: return properties_Zs(c);
	default:              return 0; // Cn Co Cs Ii Lm Lt Mc Me Nl No Sc
	};
}
