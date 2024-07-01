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
#include "test_assert.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/encoding.h>

static void
test_unbound_text_decoder()
{
	printf("testing unbound text decoder\n");

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(decoder != NULL);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_unknown_encoding()
{
	printf("testing unknown encodings\n");

	TEST_ASSERT(espeak_ng_EncodingFromName(NULL) == ESPEAKNG_ENCODING_UNKNOWN);
	TEST_ASSERT(espeak_ng_EncodingFromName("") == ESPEAKNG_ENCODING_UNKNOWN);
	TEST_ASSERT(espeak_ng_EncodingFromName("abcxyz") == ESPEAKNG_ENCODING_UNKNOWN);
	TEST_ASSERT(espeak_ng_EncodingFromName("US") == ESPEAKNG_ENCODING_UNKNOWN); // wrong case

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_UNKNOWN) == ENS_UNKNOWN_TEXT_ENCODING);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_us_ascii_encoding()
{
	printf("testing US-ASCII encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("US-ASCII") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-6") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("ANSI_X3.4-1968") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("ANSI_X3.4-1986") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_646.irv:1991") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO646-US") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("us") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("IBM367") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("cp367") == ESPEAKNG_ENCODING_US_ASCII);
	TEST_ASSERT(espeak_ng_EncodingFromName("csASCII") == ESPEAKNG_ENCODING_US_ASCII);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_US_ASCII) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_koi8_r_encoding()
{
	printf("testing KOI8-R encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("KOI8-R") == ESPEAKNG_ENCODING_KOI8_R);
	TEST_ASSERT(espeak_ng_EncodingFromName("csKOI8R") == ESPEAKNG_ENCODING_KOI8_R);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_KOI8_R) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x021a);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iscii_encoding()
{
	printf("testing ISCII encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISCII") == ESPEAKNG_ENCODING_ISCII);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xE6", 5, ESPEAKNG_ENCODING_ISCII) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xfffd);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xfffd);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x094c);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_1_encoding()
{
	printf("testing ISO-8859-1 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-1") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-1") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-1:1987") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-100") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin1") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("l1") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("IBM819") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("cp819") == ESPEAKNG_ENCODING_ISO_8859_1);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatin1") == ESPEAKNG_ENCODING_ISO_8859_1);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xDE);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_2_encoding()
{
	printf("testing ISO-8859-2 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-2") == ESPEAKNG_ENCODING_ISO_8859_2);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-2") == ESPEAKNG_ENCODING_ISO_8859_2);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-2:1987") == ESPEAKNG_ENCODING_ISO_8859_2);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-101") == ESPEAKNG_ENCODING_ISO_8859_2);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin2") == ESPEAKNG_ENCODING_ISO_8859_2);
	TEST_ASSERT(espeak_ng_EncodingFromName("l2") == ESPEAKNG_ENCODING_ISO_8859_2);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatin2") == ESPEAKNG_ENCODING_ISO_8859_2);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_2) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0162);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_3_encoding()
{
	printf("testing ISO-8859-3 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-3") == ESPEAKNG_ENCODING_ISO_8859_3);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-3") == ESPEAKNG_ENCODING_ISO_8859_3);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-3:1988") == ESPEAKNG_ENCODING_ISO_8859_3);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-109") == ESPEAKNG_ENCODING_ISO_8859_3);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin3") == ESPEAKNG_ENCODING_ISO_8859_3);
	TEST_ASSERT(espeak_ng_EncodingFromName("l3") == ESPEAKNG_ENCODING_ISO_8859_3);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatin3") == ESPEAKNG_ENCODING_ISO_8859_3);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_3) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x015C);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_4_encoding()
{
	printf("testing ISO-8859-4 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-4") == ESPEAKNG_ENCODING_ISO_8859_4);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-4") == ESPEAKNG_ENCODING_ISO_8859_4);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-4:1988") == ESPEAKNG_ENCODING_ISO_8859_4);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-110") == ESPEAKNG_ENCODING_ISO_8859_4);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin4") == ESPEAKNG_ENCODING_ISO_8859_4);
	TEST_ASSERT(espeak_ng_EncodingFromName("l4") == ESPEAKNG_ENCODING_ISO_8859_4);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatin4") == ESPEAKNG_ENCODING_ISO_8859_4);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_4) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x016A);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_5_encoding()
{
	printf("testing ISO-8859-5 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-5") == ESPEAKNG_ENCODING_ISO_8859_5);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-5") == ESPEAKNG_ENCODING_ISO_8859_5);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-5:1988") == ESPEAKNG_ENCODING_ISO_8859_5);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-144") == ESPEAKNG_ENCODING_ISO_8859_5);
	TEST_ASSERT(espeak_ng_EncodingFromName("cyrillic") == ESPEAKNG_ENCODING_ISO_8859_5);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatinCyrillic") == ESPEAKNG_ENCODING_ISO_8859_5);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_5) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x043E);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_6_encoding()
{
	printf("testing ISO-8859-6 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-6") == ESPEAKNG_ENCODING_ISO_8859_6);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-6") == ESPEAKNG_ENCODING_ISO_8859_6);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-6:1987") == ESPEAKNG_ENCODING_ISO_8859_6);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-127") == ESPEAKNG_ENCODING_ISO_8859_6);
	TEST_ASSERT(espeak_ng_EncodingFromName("ECMA-114") == ESPEAKNG_ENCODING_ISO_8859_6);
	TEST_ASSERT(espeak_ng_EncodingFromName("ASMO-708") == ESPEAKNG_ENCODING_ISO_8859_6);
	TEST_ASSERT(espeak_ng_EncodingFromName("arabic") == ESPEAKNG_ENCODING_ISO_8859_6);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatinArabic") == ESPEAKNG_ENCODING_ISO_8859_6);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDA", 5, ESPEAKNG_ENCODING_ISO_8859_6) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x063A);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_7_encoding()
{
	printf("testing ISO-8859-7 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-7") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-7") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-7:1987") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-126") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("ECMA-118") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("ELOT_928") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("greek") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("greek8") == ESPEAKNG_ENCODING_ISO_8859_7);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatinGreek") == ESPEAKNG_ENCODING_ISO_8859_7);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_7) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x03AE);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_8_encoding()
{
	printf("testing ISO-8859-8 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-8") == ESPEAKNG_ENCODING_ISO_8859_8);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-8") == ESPEAKNG_ENCODING_ISO_8859_8);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-8:1988") == ESPEAKNG_ENCODING_ISO_8859_8);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-138") == ESPEAKNG_ENCODING_ISO_8859_8);
	TEST_ASSERT(espeak_ng_EncodingFromName("hebrew") == ESPEAKNG_ENCODING_ISO_8859_8);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatinHebrew") == ESPEAKNG_ENCODING_ISO_8859_8);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xEE", 5, ESPEAKNG_ENCODING_ISO_8859_8) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x05de);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_9_encoding()
{
	printf("testing ISO-8859-9 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-9") == ESPEAKNG_ENCODING_ISO_8859_9);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-9") == ESPEAKNG_ENCODING_ISO_8859_9);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-9:1989") == ESPEAKNG_ENCODING_ISO_8859_9);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-148") == ESPEAKNG_ENCODING_ISO_8859_9);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin5") == ESPEAKNG_ENCODING_ISO_8859_9);
	TEST_ASSERT(espeak_ng_EncodingFromName("l5") == ESPEAKNG_ENCODING_ISO_8859_9);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatin5") == ESPEAKNG_ENCODING_ISO_8859_9);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_9) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x015e);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_10_encoding()
{
	printf("testing ISO-8859-10 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-10") == ESPEAKNG_ENCODING_ISO_8859_10);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-10") == ESPEAKNG_ENCODING_ISO_8859_10);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-10:1992") == ESPEAKNG_ENCODING_ISO_8859_10);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-157") == ESPEAKNG_ENCODING_ISO_8859_10);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin6") == ESPEAKNG_ENCODING_ISO_8859_10);
	TEST_ASSERT(espeak_ng_EncodingFromName("l6") == ESPEAKNG_ENCODING_ISO_8859_10);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISOLatin6") == ESPEAKNG_ENCODING_ISO_8859_10);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_10) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x00de);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_11_encoding()
{
	printf("testing ISO-8859-11 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-11") == ESPEAKNG_ENCODING_ISO_8859_11);
	TEST_ASSERT(espeak_ng_EncodingFromName("TIS-620") == ESPEAKNG_ENCODING_ISO_8859_11);
	TEST_ASSERT(espeak_ng_EncodingFromName("csTIS620") == ESPEAKNG_ENCODING_ISO_8859_11);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xEE", 5, ESPEAKNG_ENCODING_ISO_8859_11) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0e4e);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_13_encoding()
{
	printf("testing ISO-8859-13 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-13") == ESPEAKNG_ENCODING_ISO_8859_13);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISO885913") == ESPEAKNG_ENCODING_ISO_8859_13);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xEE", 5, ESPEAKNG_ENCODING_ISO_8859_13) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x012b);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_14_encoding()
{
	printf("testing ISO-8859-14 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-14") == ESPEAKNG_ENCODING_ISO_8859_14);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-14") == ESPEAKNG_ENCODING_ISO_8859_14);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-14:1998") == ESPEAKNG_ENCODING_ISO_8859_14);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-199") == ESPEAKNG_ENCODING_ISO_8859_14);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-celtic") == ESPEAKNG_ENCODING_ISO_8859_14);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin8") == ESPEAKNG_ENCODING_ISO_8859_14);
	TEST_ASSERT(espeak_ng_EncodingFromName("l8") == ESPEAKNG_ENCODING_ISO_8859_14);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISO885914") == ESPEAKNG_ENCODING_ISO_8859_14);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_14) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0176);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_15_encoding()
{
	printf("testing ISO-8859-15 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-15") == ESPEAKNG_ENCODING_ISO_8859_15);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-15") == ESPEAKNG_ENCODING_ISO_8859_15);
	TEST_ASSERT(espeak_ng_EncodingFromName("Latin-9") == ESPEAKNG_ENCODING_ISO_8859_15);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISO885915") == ESPEAKNG_ENCODING_ISO_8859_15);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xBE", 5, ESPEAKNG_ENCODING_ISO_8859_15) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0178);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_8859_16_encoding()
{
	printf("testing ISO-8859-16 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-8859-16") == ESPEAKNG_ENCODING_ISO_8859_16);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-16") == ESPEAKNG_ENCODING_ISO_8859_16);
	TEST_ASSERT(espeak_ng_EncodingFromName("ISO_8859-16:2001") == ESPEAKNG_ENCODING_ISO_8859_16);
	TEST_ASSERT(espeak_ng_EncodingFromName("iso-ir-226") == ESPEAKNG_ENCODING_ISO_8859_16);
	TEST_ASSERT(espeak_ng_EncodingFromName("latin10") == ESPEAKNG_ENCODING_ISO_8859_16);
	TEST_ASSERT(espeak_ng_EncodingFromName("l10") == ESPEAKNG_ENCODING_ISO_8859_16);
	TEST_ASSERT(espeak_ng_EncodingFromName("csISO885916") == ESPEAKNG_ENCODING_ISO_8859_16);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aG\x92\xA0\xDE", 5, ESPEAKNG_ENCODING_ISO_8859_16) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x92);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x021a);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_utf_8_encoding()
{
	printf("testing UTF-8 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("UTF-8") == ESPEAKNG_ENCODING_UTF_8);
	TEST_ASSERT(espeak_ng_EncodingFromName("csUTF8") == ESPEAKNG_ENCODING_UTF_8);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	// 1-byte UTF-8 sequences
	TEST_ASSERT(text_decoder_decode_string(decoder, "\x0D\x1E\x20\x35\x42\x57\x65\x77", 8, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x000D);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x001E);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0020);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0035);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0042);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0057);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0065);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0077);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// UTF-8 tail bytes without an initial length indicator character
	TEST_ASSERT(text_decoder_decode_string(decoder, "\x84\x92\xA8\xB5", 4, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// 2-byte UTF-8 sequences
	TEST_ASSERT(text_decoder_decode_string(decoder, "\xC2\xA0\xD0\xB0\xC5\x65\xC2\xA0", 7, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x00A0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0430);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // \x65 is not a continuation byte, so \xC5 is invalid
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0065);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // incomplete: \xA0 is past the end of the string
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// 3-byte UTF-8 sequences
	TEST_ASSERT(text_decoder_decode_string(decoder, "\xE4\xBA\x8C\xE8\x42\xE2\x93\x44\xE4\xA0\x80", 9, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x4E8C);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // \x42 is not a continuation byte, so \xE8 is invalid
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0042);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // \x44 is not a continuation byte, so \xE2\x93 is invalid
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0044);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // incomplete: \xA0 is past the end of the string
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// 4-byte UTF-8 sequences
	TEST_ASSERT(text_decoder_decode_string(decoder, "\xF0\x90\x8C\x82\xF4\x8F\xBF\xBF\xF3\x61\xF3\xA5\x32\xF3\x87\xB2\x36\xF1\xA0\x80\x80", 18, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x10302);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x10FFFF);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // \x61 is not a continuation byte, so \xF3 is invalid
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0061);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // \x32 is not a continuation byte, so \xF3\xA5 is invalid
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0032);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // \x36 is not a continuation byte, so \xF3\x87\xB2 is invalid
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x0036);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD); // incomplete: \xA0 is past the end of the string
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// out of range (> 0x10FFFF)
	TEST_ASSERT(text_decoder_decode_string(decoder, "\xF4\x90\x80\x80", 4, ESPEAKNG_ENCODING_UTF_8) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_iso_10646_ucs_2_encoding()
{
	printf("testing ISO-10646-UCS-2 encoding\n");

	TEST_ASSERT(espeak_ng_EncodingFromName("ISO-10646-UCS-2") == ESPEAKNG_ENCODING_ISO_10646_UCS_2);
	TEST_ASSERT(espeak_ng_EncodingFromName("csUnicode") == ESPEAKNG_ENCODING_ISO_10646_UCS_2);

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "a\00G\00\xA0\00\x22\x21\x23\x21", 9, ESPEAKNG_ENCODING_ISO_10646_UCS_2) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x2122);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xFFFD);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_char_decoder()
{
	printf("testing char decoder\n");

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	// null string
	TEST_ASSERT(text_decoder_decode_string(decoder, NULL, -1, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);
	TEST_ASSERT(text_decoder_getc(decoder) == 0);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// string length
	TEST_ASSERT(text_decoder_decode_string(decoder, "aG", -1, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_wchar_decoder()
{
	printf("testing wchar_t decoder\n");

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	// null string
	TEST_ASSERT(text_decoder_decode_wstring(decoder, NULL, -1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);
	TEST_ASSERT(text_decoder_getc(decoder) == 0);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// wide-character string
	TEST_ASSERT(text_decoder_decode_wstring(decoder, L"aG\xA0\x2045", 4) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x2045);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// string length
	TEST_ASSERT(text_decoder_decode_wstring(decoder, L"aG\xA0\x2045", -1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0x2045);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_auto_decoder()
{
	printf("testing auto decoder (UTF-8 + codepage-based fallback)\n");

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	// null string
	TEST_ASSERT(text_decoder_decode_string_auto(decoder, NULL, -1, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);
	TEST_ASSERT(text_decoder_getc(decoder) == 0);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// UTF-8
	TEST_ASSERT(text_decoder_decode_string_auto(decoder, "aG\xC2\xA0 ", 5, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == ' ');
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// ISO-8859-1
	TEST_ASSERT(text_decoder_decode_string_auto(decoder, "aG\240f", 4, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0xA0);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'f');
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// string length
	TEST_ASSERT(text_decoder_decode_string_auto(decoder, "aG", -1, ESPEAKNG_ENCODING_ISO_8859_1) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 0);
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

static void
test_peekc()
{
	printf("testing peekc\n");

	espeak_ng_TEXT_DECODER *decoder = create_text_decoder();

	TEST_ASSERT(text_decoder_decode_string(decoder, "aGd", 3, ESPEAKNG_ENCODING_US_ASCII) == ENS_OK);
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'a');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_peekc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'G');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_peekc(decoder) == 'd');
	TEST_ASSERT(text_decoder_eof(decoder) == 0);
	TEST_ASSERT(text_decoder_getc(decoder) == 'd');
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	// Calling peekc past the end of the buffer.
	TEST_ASSERT(text_decoder_peekc(decoder) == '\0');
	TEST_ASSERT(text_decoder_eof(decoder) == 1);

	destroy_text_decoder(decoder);
}

int
main(int argc, char **argv)
{
	(void)argc; // unused parameter
	(void)argv; // unused parameter

	test_unbound_text_decoder();
	test_unknown_encoding();

	test_us_ascii_encoding();
	test_koi8_r_encoding();
	test_iscii_encoding();

	test_iso_8859_1_encoding();
	test_iso_8859_2_encoding();
	test_iso_8859_3_encoding();
	test_iso_8859_4_encoding();
	test_iso_8859_5_encoding();
	test_iso_8859_6_encoding();
	test_iso_8859_7_encoding();
	test_iso_8859_8_encoding();
	test_iso_8859_9_encoding();
	test_iso_8859_10_encoding();
	test_iso_8859_11_encoding();
	// ISO-8859-12 is not a valid encoding.
	test_iso_8859_13_encoding();
	test_iso_8859_14_encoding();
	test_iso_8859_15_encoding();
	test_iso_8859_16_encoding();

	test_utf_8_encoding();
	test_iso_10646_ucs_2_encoding();

	test_char_decoder();
	test_wchar_decoder();
	test_auto_decoder();

	test_peekc();

	return EXIT_SUCCESS;
}
