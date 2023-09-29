/*
 * Copyright (C) 2022 Ulrich Müller <ulm@gentoo.org>
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
 *
 *
 * Alternatively, at your option, you can distribute this file under
 * the terms of the 2-clause BSD license.
 */

#include "config.h"
#include "test_assert.h"
#include <math.h>
#include <stdlib.h>
#include "ieee80.h"

int
main(int argc, char **argv)
{
	(void)argc; (void)argv;	/* unused */

	/* Define only constants that can be exactly represented both in
	   decimal and in binary, in order to avoid rounding errors */
	unsigned char in[][10] = {
		{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* 0 */
		{ 0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* 1 */
		{ 0xbf,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* -1 */
		{ 0x40,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* 2 */
		{ 0x3f,0xfe,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* 0.5 */
		{ 0x3f,0xf7,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* 0.005859375 */
		{ 0x40,0x22,0x98,0x76,0x54,0x32,0x10,0x00,0x00,0x00 }, /* 40926266145 */
		{ 0x7f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* inf */
		{ 0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, /* -inf */
		{ 0x7f,0xff,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }  /* nan */
	};

	TEST_ASSERT(ieee_extended_to_double(in[0]) == 0.);
	TEST_ASSERT(ieee_extended_to_double(in[1]) == 1.);
	TEST_ASSERT(ieee_extended_to_double(in[2]) == -1.);
	TEST_ASSERT(ieee_extended_to_double(in[3]) == 2.);
	TEST_ASSERT(ieee_extended_to_double(in[4]) == 0.5);
	TEST_ASSERT(ieee_extended_to_double(in[5]) == 0.005859375);
	TEST_ASSERT(ieee_extended_to_double(in[6]) == 40926266145.);
#ifdef INFINITY
	TEST_ASSERT(ieee_extended_to_double(in[7]) == INFINITY);
	TEST_ASSERT(ieee_extended_to_double(in[8]) == -INFINITY);
#endif
#ifdef NAN
	TEST_ASSERT(isnan(ieee_extended_to_double(in[9])));
#endif

	return EXIT_SUCCESS;
}
