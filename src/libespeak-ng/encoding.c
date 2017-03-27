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
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <stdint.h>
#include <stdlib.h>

#include <espeak-ng/espeak_ng.h>

#include "speech.h"
#include "encoding.h"

// http://www.iana.org/assignments/character-sets/character-sets.xhtml
MNEM_TAB mnem_encoding[] = {
	{ "ANSI_X3.4-1968",   ESPEAKNG_ENCODING_US_ASCII },
	{ "ANSI_X3.4-1986",   ESPEAKNG_ENCODING_US_ASCII },
	{ "IBM367",           ESPEAKNG_ENCODING_US_ASCII },
	{ "IBM819",           ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO_646.irv:1991", ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO_8859-1",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO_8859-2",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "ISO646-US",        ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO-8859-1",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO-8859-1:1987",  ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO-8859-2",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "ISO-8859-2:1987",  ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "US-ASCII",         ESPEAKNG_ENCODING_US_ASCII },
	{ "cp367",            ESPEAKNG_ENCODING_US_ASCII },
	{ "cp819",            ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "csASCII",          ESPEAKNG_ENCODING_US_ASCII },
	{ "csISOLatin1",      ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "csISOLatin2",      ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "iso-ir-6",         ESPEAKNG_ENCODING_US_ASCII },
	{ "iso-ir-100",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "iso-ir-101",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "latin1",           ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "latin2",           ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "l1",               ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "l2",               ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "us",               ESPEAKNG_ENCODING_US_ASCII },
	{ NULL,               ESPEAKNG_ENCODING_UNKNOWN }
};

#pragma GCC visibility push(default)

espeak_ng_ENCODING
espeak_ng_EncodingFromName(const char *encoding)
{
	return LookupMnem(mnem_encoding, encoding);
}

#pragma GCC visibility pop

struct espeak_ng_TEXT_DECODER_
{
	const char *current;
	const char *end;

	uint32_t (*get)(espeak_ng_TEXT_DECODER *decoder);
	uint32_t *codepage;
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-2.TXT
static const uint32_t ISO_8859_2[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // a0
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // a8
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // b0
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // b8
	0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7, // a0
	0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b, // a8
	0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7, // b0
	0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c, // b8
	0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7, // c0
	0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e, // c8
	0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7, // d0
	0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df, // d8
	0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7, // e0
	0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f, // e8
	0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7, // f0
	0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9, // f8
};

static uint32_t
text_decoder_getc_us_ascii(espeak_ng_TEXT_DECODER *decoder)
{
	uint8_t c = *decoder->current++ & 0xFF;
	return (c >= 0x80) ? 0xFFFD : c;
}

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-1.TXT
static uint32_t
text_decoder_getc_iso_8859_1(espeak_ng_TEXT_DECODER *decoder)
{
	return *decoder->current++ & 0xFF;
}

static uint32_t
text_decoder_getc_codepage(espeak_ng_TEXT_DECODER *decoder)
{
	uint8_t c = *decoder->current++ & 0xFF;
	return (c >= 0x80) ? decoder->codepage[c - 0x80] : c;
}

espeak_ng_TEXT_DECODER *
create_text_decoder(void)
{
	espeak_ng_TEXT_DECODER *decoder = malloc(sizeof(espeak_ng_TEXT_DECODER));
	if (!decoder) return NULL;

	decoder->current = NULL;
	decoder->end = NULL;
	decoder->get = NULL;
	decoder->codepage = NULL;
	return decoder;
}

void
destroy_text_decoder(espeak_ng_TEXT_DECODER *decoder)
{
	if (decoder) free(decoder);
}

static int
initialize_encoding(espeak_ng_TEXT_DECODER *decoder,
                    espeak_ng_ENCODING encoding)
{
	switch (encoding)
	{
	case ESPEAKNG_ENCODING_US_ASCII:
		decoder->get = text_decoder_getc_us_ascii;
		decoder->codepage = NULL;
		break;
	case ESPEAKNG_ENCODING_ISO_8859_1:
		decoder->get = text_decoder_getc_iso_8859_1;
		decoder->codepage = NULL;
		break;
	case ESPEAKNG_ENCODING_ISO_8859_2:
		decoder->get = text_decoder_getc_codepage;
		decoder->codepage = ISO_8859_2;
		break;
	default:
		return 0;
	}
	return 1;
}

espeak_ng_STATUS
text_decoder_decode_string(espeak_ng_TEXT_DECODER *decoder,
                           const char *string,
                           int length,
                           espeak_ng_ENCODING encoding)
{
	if (!initialize_encoding(decoder, encoding))
		return ENS_UNKNOWN_TEXT_ENCODING;

	decoder->current = string;
	decoder->end = string + length;
	return ENS_OK;
}

int
text_decoder_eof(espeak_ng_TEXT_DECODER *decoder)
{
	return decoder->current == decoder->end;
}

uint32_t
text_decoder_getc(espeak_ng_TEXT_DECODER *decoder)
{
	return decoder->get(decoder);
}
