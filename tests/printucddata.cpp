/*
 * Copyright (C) 2012-2013 Reece H. Dunn
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

#include <stdio.h>

int main()
{
	for (ucd::codepoint_t c = 0; c <= 0x10FFFF; ++c)
	{
		const char *script = ucd::get_script_string(ucd::lookup_script(c));
		const char *category = ucd::get_category_string(ucd::lookup_category(c));
		const char *category_group = ucd::get_category_group_string(ucd::lookup_category_group(c));
		ucd::codepoint_t upper = ucd::toupper(c);
		ucd::codepoint_t lower = ucd::tolower(c);
		ucd::codepoint_t title = ucd::totitle(c);
		const char *whitespace = ucd::isspace(c) ? "White_Space" : "";
		printf("%06X %s %s %s %06X %06X %06X %s\n",
		       c, script, category_group, category,
		       upper, lower, title,
		       whitespace);
	}
	return 0;
}
