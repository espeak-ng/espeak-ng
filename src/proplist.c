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
	case UCD_CATEGORY_Zl:
	case UCD_CATEGORY_Zp:
	case UCD_CATEGORY_Zs:
		return UCD_PROPERTY_WHITE_SPACE;
	default:
		return 0;
	};
}
