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

static int properties_Pd(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0000:
		if (c == 0x002D)                return UCD_PROPERTY_DASH;
		break;
	case 0x0500:
		if (c == 0x058A)                return UCD_PROPERTY_DASH;
		if (c == 0x05BE)                return UCD_PROPERTY_DASH;
		break;
	case 0x1400:
		if (c == 0x1400)                return UCD_PROPERTY_DASH;
		break;
	case 0x1800:
		if (c == 0x1806)                return UCD_PROPERTY_DASH;
		break;
	case 0x2000:
		if (c == 0x2010)                return UCD_PROPERTY_DASH;
		if (c == 0x2011)                return UCD_PROPERTY_DASH | UCD_PROPERTY_NO_BREAK;
		if (c >= 0x2012 && c <= 0x2015) return UCD_PROPERTY_DASH;
		break;
	case 0x2E00:
		if (c == 0x2E17)                return UCD_PROPERTY_DASH;
		if (c == 0x2E1A)                return UCD_PROPERTY_DASH;
		if (c >= 0x2E3A && c <= 0x2E3B) return UCD_PROPERTY_DASH;
		if (c == 0x2E40)                return UCD_PROPERTY_DASH;
		break;
	case 0x3000:
		if (c == 0x301C)                return UCD_PROPERTY_DASH;
		if (c == 0x3030)                return UCD_PROPERTY_DASH;
		if (c == 0x30A0)                return UCD_PROPERTY_DASH;
		break;
	case 0xFE00:
		if (c >= 0xFE31 && c <= 0xFE32) return UCD_PROPERTY_DASH;
		if (c == 0xFE58)                return UCD_PROPERTY_DASH;
		if (c == 0xFE63)                return UCD_PROPERTY_DASH;
		break;
	case 0xFF00:
		if (c == 0xFF0D)                return UCD_PROPERTY_DASH;
		break;
	}
	return 0;
}

static int properties_Po(codepoint_t c)
{
	switch (c & 0xFFFFFF00)
	{
	case 0x0F00:
		if (c == 0x0F0C)                return UCD_PROPERTY_NO_BREAK;
		break;
	case 0x2000:
		if (c == 0x2053)                return UCD_PROPERTY_DASH;
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

ucd_property ucd_properties(codepoint_t c, ucd_category category)
{
	switch (category)
	{
	case UCD_CATEGORY_Cc:
		switch (c)
		{
		case 0x09: // CHARACTER TABULATION
		case 0x0A: // LINE FEED
		case 0x0B: // LINE TABULATION
		case 0x0C: // FORM FEED
		case 0x0D: // CARRIAGE RETURN
		case 0x85: // NEXT LINE
			return UCD_PROPERTY_WHITE_SPACE;
		}
		return 0;
	case UCD_CATEGORY_Cf:
		if (c == 0x061C)                return UCD_PROPERTY_BIDI_CONTROL;
		if (c >= 0x200C && c <= 0x200D) return UCD_PROPERTY_JOIN_CONTROL;
		if (c >= 0x200E && c <= 0x200F) return UCD_PROPERTY_BIDI_CONTROL;
		if (c >= 0x202A && c <= 0x202E) return UCD_PROPERTY_BIDI_CONTROL;
		if (c >= 0x2066 && c <= 0x2069) return UCD_PROPERTY_BIDI_CONTROL;
		return 0;
	case UCD_CATEGORY_Pd: return properties_Pd(c);
	case UCD_CATEGORY_Po: return properties_Po(c);
	case UCD_CATEGORY_Sm: return properties_Sm(c);
	case UCD_CATEGORY_Zl:
	case UCD_CATEGORY_Zp:
		return UCD_PROPERTY_WHITE_SPACE;
	case UCD_CATEGORY_Zs:
		switch (c)
		{
		case 0x00A0: // NO-BREAK SPACE
		case 0x2007: // FIGURE SPACE
		case 0x202F: // NARROW NO-BREAK SPACE
			return UCD_PROPERTY_WHITE_SPACE | UCD_PROPERTY_NO_BREAK;
		}
		return UCD_PROPERTY_WHITE_SPACE;
	default:
		return 0;
	};
}
