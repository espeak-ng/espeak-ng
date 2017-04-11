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
		if (c >= 0x2066 && c <= 0x2069) return UCD_PROPERTY_BIDI_CONTROL;
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
		if (c == 0xFE63)                return UCD_PROPERTY_DASH | UCD_PROPERTY_HYPHEN;
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
		if (c == 0x0022)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0x0027)                return UCD_PROPERTY_QUOTATION_MARK;
		break;
	case 0x0F00:
		if (c == 0x0F0C)                return UCD_PROPERTY_NO_BREAK;
		break;
	case 0x2000:
		if (c == 0x2053)                return UCD_PROPERTY_DASH;
		break;
	case 0x3000:
		if (c == 0x30FB)                return UCD_PROPERTY_HYPHEN;
		break;
	case 0xFF00:
		if (c == 0xFF02)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0xFF07)                return UCD_PROPERTY_QUOTATION_MARK;
		if (c == 0xFF65)                return UCD_PROPERTY_HYPHEN;
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
	case UCD_CATEGORY_Pd: return properties_Pd(c);
	case UCD_CATEGORY_Pe: return properties_Pe(c);
	case UCD_CATEGORY_Pf: return properties_Pf(c);
	case UCD_CATEGORY_Pi: return properties_Pi(c);
	case UCD_CATEGORY_Po: return properties_Po(c);
	case UCD_CATEGORY_Ps: return properties_Ps(c);
	case UCD_CATEGORY_Sm: return properties_Sm(c);
	case UCD_CATEGORY_Zl: return UCD_PROPERTY_WHITE_SPACE;
	case UCD_CATEGORY_Zp: return UCD_PROPERTY_WHITE_SPACE;
	case UCD_CATEGORY_Zs: return properties_Zs(c);
	default:              return 0;
	};
}
