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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <espeak-ng/espeak_ng.h>
#include "encoding.h"

void
test_unbound_text_decoder()
{
	printf("testing unbound text decoder\n");

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(decoder != NULL);
	assert(text_decoder_eof(decoder) == 1);
	assert(text_decoder_getc(decoder) == 0);

	destroy_text_decoder(decoder);
}

void
test_unknown_encoding()
{
	printf("testing unknown encodings\n");

	assert(espeak_ng_EncodingFromName(NULL) == ESPEAKNG_ENCODING_UNKNOWN);
	assert(espeak_ng_EncodingFromName("") == ESPEAKNG_ENCODING_UNKNOWN);
	assert(espeak_ng_EncodingFromName("abcxyz") == ESPEAKNG_ENCODING_UNKNOWN);
	assert(espeak_ng_EncodingFromName("US") == ESPEAKNG_ENCODING_UNKNOWN); // wrong case
}

void
test_us_ascii_encoding()
{
	printf("testing US-ASCII encoding\n");

	assert(espeak_ng_EncodingFromName("US-ASCII") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("iso-ir-6") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("ANSI_X3.4-1968") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("ANSI_X3.4-1986") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("ISO_646.irv:1991") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("ISO646-US") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("us") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("IBM367") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("cp367") == ESPEAKNG_ENCODING_US_ASCII);
	assert(espeak_ng_EncodingFromName("csASCII") == ESPEAKNG_ENCODING_US_ASCII);
}

int
main(int argc, char **argv)
{
	test_unbound_text_decoder();
	test_unknown_encoding();
	test_us_ascii_encoding();
	printf("done\n");

	return EXIT_SUCCESS;
}
