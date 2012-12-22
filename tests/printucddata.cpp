/*
 * Copyright (C) 2012 Reece H. Dunn
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

const char *get_category_string(ucd::category c)
{
	using namespace ucd;
	switch (c)
	{
	case Cc: return "Cc";
	case Cf: return "Cf";
	case Cn: return "Cn";
	case Co: return "Co";
	case Cs: return "Cs";
	case Ii: return "Ii";
	case Ll: return "Ll";
	case Lm: return "Lm";
	case Lo: return "Lo";
	case Lt: return "Lt";
	case Lu: return "Lu";
	case Mc: return "Mc";
	case Me: return "Me";
	case Mn: return "Mn";
	case Nd: return "Nd";
	case Nl: return "Nl";
	case No: return "No";
	case Pc: return "Pc";
	case Pd: return "Pd";
	case Pe: return "Pe";
	case Pf: return "Pf";
	case Pi: return "Pi";
	case Po: return "Po";
	case Ps: return "Ps";
	case Sc: return "Sc";
	case Sk: return "Sk";
	case Sm: return "Sm";
	case So: return "So";
	case Zl: return "Zl";
	case Zp: return "Zp";
	case Zs: return "Zs";
	default: return "--";
	}
}

int main()
{
	for (ucd::codepoint_t c = 0; c <= 0x10FFFF; ++c)
	{
		const char *category = get_category_string(ucd::lookup_category(c));
		ucd::codepoint_t upper = ucd::toupper(c);
		ucd::codepoint_t lower = ucd::tolower(c);
		ucd::codepoint_t title = ucd::totitle(c);
		printf("%06X %s %06X %06X %06X\n", c, category, upper, lower, title);
	}
	return 0;
}
