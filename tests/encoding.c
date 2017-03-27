/*
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
 * along with this program; if not, write see:
 *             <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <espeak-ng/espeak_ng.h>

void
test_unknown_encoding()
{
	printf("testing unknown encoding\n");

	assert(espeak_ng_EncodingFromName("") == ESPEAKNG_ENCODING_UNKNOWN);
	assert(espeak_ng_EncodingFromName("abcxyz") == ESPEAKNG_ENCODING_UNKNOWN);
}

int
main(int argc, char **argv)
{
	test_unknown_encoding();
	printf("done\n");

	return EXIT_SUCCESS;
}
