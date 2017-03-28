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

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_UNKNOWN) == ENS_UNKNOWN_TEXT_ENCODING);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
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

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_US_ASCII) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xFFFD);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xFFFD);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xFFFD);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_1_encoding()
{
	printf("testing ISO-8859-1 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-1:1987") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("ISO-8859-1") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("ISO_8859-1") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("iso-ir-100") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("latin1") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("l1") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("IBM819") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("cp819") == ESPEAKNG_ENCODING_ISO_8859_1);
	assert(espeak_ng_EncodingFromName("csISOLatin1") == ESPEAKNG_ENCODING_ISO_8859_1);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xDE);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_2_encoding()
{
	printf("testing ISO-8859-2 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-2:1987") == ESPEAKNG_ENCODING_ISO_8859_2);
	assert(espeak_ng_EncodingFromName("ISO-8859-2") == ESPEAKNG_ENCODING_ISO_8859_2);
	assert(espeak_ng_EncodingFromName("ISO_8859-2") == ESPEAKNG_ENCODING_ISO_8859_2);
	assert(espeak_ng_EncodingFromName("iso-ir-101") == ESPEAKNG_ENCODING_ISO_8859_2);
	assert(espeak_ng_EncodingFromName("latin2") == ESPEAKNG_ENCODING_ISO_8859_2);
	assert(espeak_ng_EncodingFromName("l2") == ESPEAKNG_ENCODING_ISO_8859_2);
	assert(espeak_ng_EncodingFromName("csISOLatin2") == ESPEAKNG_ENCODING_ISO_8859_2);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_2) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x0162);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_3_encoding()
{
	printf("testing ISO-8859-3 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-3:1988") == ESPEAKNG_ENCODING_ISO_8859_3);
	assert(espeak_ng_EncodingFromName("ISO-8859-3") == ESPEAKNG_ENCODING_ISO_8859_3);
	assert(espeak_ng_EncodingFromName("ISO_8859-3") == ESPEAKNG_ENCODING_ISO_8859_3);
	assert(espeak_ng_EncodingFromName("iso-ir-109") == ESPEAKNG_ENCODING_ISO_8859_3);
	assert(espeak_ng_EncodingFromName("latin3") == ESPEAKNG_ENCODING_ISO_8859_3);
	assert(espeak_ng_EncodingFromName("l3") == ESPEAKNG_ENCODING_ISO_8859_3);
	assert(espeak_ng_EncodingFromName("csISOLatin3") == ESPEAKNG_ENCODING_ISO_8859_3);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_3) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x015C);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_4_encoding()
{
	printf("testing ISO-8859-4 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-4:1988") == ESPEAKNG_ENCODING_ISO_8859_4);
	assert(espeak_ng_EncodingFromName("ISO-8859-4") == ESPEAKNG_ENCODING_ISO_8859_4);
	assert(espeak_ng_EncodingFromName("ISO_8859-4") == ESPEAKNG_ENCODING_ISO_8859_4);
	assert(espeak_ng_EncodingFromName("iso-ir-110") == ESPEAKNG_ENCODING_ISO_8859_4);
	assert(espeak_ng_EncodingFromName("latin4") == ESPEAKNG_ENCODING_ISO_8859_4);
	assert(espeak_ng_EncodingFromName("l4") == ESPEAKNG_ENCODING_ISO_8859_4);
	assert(espeak_ng_EncodingFromName("csISOLatin4") == ESPEAKNG_ENCODING_ISO_8859_4);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_4) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x016A);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_5_encoding()
{
	printf("testing ISO-8859-5 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-5:1988") == ESPEAKNG_ENCODING_ISO_8859_5);
	assert(espeak_ng_EncodingFromName("ISO-8859-5") == ESPEAKNG_ENCODING_ISO_8859_5);
	assert(espeak_ng_EncodingFromName("ISO_8859-5") == ESPEAKNG_ENCODING_ISO_8859_5);
	assert(espeak_ng_EncodingFromName("iso-ir-144") == ESPEAKNG_ENCODING_ISO_8859_5);
	assert(espeak_ng_EncodingFromName("cyrillic") == ESPEAKNG_ENCODING_ISO_8859_5);
	assert(espeak_ng_EncodingFromName("csISOLatinCyrillic") == ESPEAKNG_ENCODING_ISO_8859_5);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_5) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x043E);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_6_encoding()
{
	printf("testing ISO-8859-6 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-6:1987") == ESPEAKNG_ENCODING_ISO_8859_6);
	assert(espeak_ng_EncodingFromName("ISO-8859-6") == ESPEAKNG_ENCODING_ISO_8859_6);
	assert(espeak_ng_EncodingFromName("ISO_8859-6") == ESPEAKNG_ENCODING_ISO_8859_6);
	assert(espeak_ng_EncodingFromName("iso-ir-127") == ESPEAKNG_ENCODING_ISO_8859_6);
	assert(espeak_ng_EncodingFromName("ECMA-114") == ESPEAKNG_ENCODING_ISO_8859_6);
	assert(espeak_ng_EncodingFromName("ASMO-708") == ESPEAKNG_ENCODING_ISO_8859_6);
	assert(espeak_ng_EncodingFromName("arabic") == ESPEAKNG_ENCODING_ISO_8859_6);
	assert(espeak_ng_EncodingFromName("csISOLatinArabic") == ESPEAKNG_ENCODING_ISO_8859_6);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDA", 5, ESPEAKNG_ENCODING_ISO_8859_6) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x063A);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_7_encoding()
{
	printf("testing ISO-8859-7 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-7:1987") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("ISO-8859-7") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("ISO_8859-7") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("iso-ir-126") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("ECMA-118") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("ELOT_928") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("greek") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("greek8") == ESPEAKNG_ENCODING_ISO_8859_7);
	assert(espeak_ng_EncodingFromName("csISOLatinGreek") == ESPEAKNG_ENCODING_ISO_8859_7);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_7) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x03AE);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_8_encoding()
{
	printf("testing ISO-8859-8 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-8:1988") == ESPEAKNG_ENCODING_ISO_8859_8);
	assert(espeak_ng_EncodingFromName("ISO-8859-8") == ESPEAKNG_ENCODING_ISO_8859_8);
	assert(espeak_ng_EncodingFromName("ISO_8859-8") == ESPEAKNG_ENCODING_ISO_8859_8);
	assert(espeak_ng_EncodingFromName("iso-ir-138") == ESPEAKNG_ENCODING_ISO_8859_8);
	assert(espeak_ng_EncodingFromName("hebrew") == ESPEAKNG_ENCODING_ISO_8859_8);
	assert(espeak_ng_EncodingFromName("csISOLatinHebrew") == ESPEAKNG_ENCODING_ISO_8859_8);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xEE", 5, ESPEAKNG_ENCODING_ISO_8859_8) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x05de);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

void
test_iso_8859_9_encoding()
{
	printf("testing ISO-8859-9 encoding\n");

	assert(espeak_ng_EncodingFromName("ISO-8859-9:1989") == ESPEAKNG_ENCODING_ISO_8859_9);
	assert(espeak_ng_EncodingFromName("ISO-8859-9") == ESPEAKNG_ENCODING_ISO_8859_9);
	assert(espeak_ng_EncodingFromName("ISO_8859-9") == ESPEAKNG_ENCODING_ISO_8859_9);
	assert(espeak_ng_EncodingFromName("iso-ir-148") == ESPEAKNG_ENCODING_ISO_8859_9);
	assert(espeak_ng_EncodingFromName("latin5") == ESPEAKNG_ENCODING_ISO_8859_9);
	assert(espeak_ng_EncodingFromName("l5") == ESPEAKNG_ENCODING_ISO_8859_9);
	assert(espeak_ng_EncodingFromName("csISOLatin5") == ESPEAKNG_ENCODING_ISO_8859_9);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	assert(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_9) == ENS_OK);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'a');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 'G');
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x92);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0xA0);
	assert(text_decoder_eof(decoder) == 0);
	assert(text_decoder_getc(decoder) == 0x015e);
	assert(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

int
main(int argc, char **argv)
{
	test_unbound_text_decoder();
	test_unknown_encoding();
	test_us_ascii_encoding();
	test_iso_8859_1_encoding();
	test_iso_8859_2_encoding();
	test_iso_8859_3_encoding();
	test_iso_8859_4_encoding();
	test_iso_8859_5_encoding();
	test_iso_8859_6_encoding();
	test_iso_8859_7_encoding();
	test_iso_8859_8_encoding();
	test_iso_8859_9_encoding();
	printf("done\n");

	return EXIT_SUCCESS;
}
