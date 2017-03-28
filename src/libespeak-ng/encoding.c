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
	{ "ISO_8859-3",       ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "ISO_8859-4",       ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "ISO_8859-5",       ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "ISO646-US",        ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO-8859-1",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO-8859-1:1987",  ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO-8859-2",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "ISO-8859-2:1987",  ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "ISO-8859-3",       ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "ISO-8859-3:1988",  ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "ISO-8859-4",       ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "ISO-8859-4:1988",  ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "ISO-8859-5",       ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "ISO-8859-5:1988",  ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "US-ASCII",         ESPEAKNG_ENCODING_US_ASCII },
	{ "cp367",            ESPEAKNG_ENCODING_US_ASCII },
	{ "cp819",            ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "csASCII",          ESPEAKNG_ENCODING_US_ASCII },
	{ "csISOLatin1",      ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "csISOLatin2",      ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "csISOLatin3",      ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "csISOLatin4",      ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "csISOLatinCyrillic",ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "cyrillic",         ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "iso-ir-6",         ESPEAKNG_ENCODING_US_ASCII },
	{ "iso-ir-100",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "iso-ir-101",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "iso-ir-109",       ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "iso-ir-110",       ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "iso-ir-144",       ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "latin1",           ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "latin2",           ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "latin3",           ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "latin4",           ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "l1",               ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "l2",               ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "l3",               ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "l4",               ESPEAKNG_ENCODING_ISO_8859_4 },
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
	const uint32_t *codepage;
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-2.TXT
static const uint32_t ISO_8859_2[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
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

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-3.TXT
static const uint32_t ISO_8859_3[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0126, 0x02d8, 0x00a3, 0x00a4, 0x0000, 0x0124, 0x00a7, // a0
	0x00a8, 0x0130, 0x015e, 0x011e, 0x0134, 0x00ad, 0x0000, 0x017b, // a8
	0x00b0, 0x0127, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x0125, 0x00b7, // b0
	0x00b8, 0x0131, 0x015f, 0x011f, 0x0135, 0x00bd, 0x0000, 0x017c, // b8
	0x00c0, 0x00c1, 0x00c2, 0x0000, 0x00c4, 0x010a, 0x0108, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0x0000, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x0120, 0x00d6, 0x00d7, // d0
	0x011c, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x016c, 0x015c, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0x0000, 0x00e4, 0x010b, 0x0109, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0x0000, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x0121, 0x00f6, 0x00f7, // f0
	0x011d, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x016d, 0x015d, 0x02d9, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-4.TXT
static const uint32_t ISO_8859_4[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0104, 0x0138, 0x0156, 0x00a4, 0x0128, 0x013b, 0x00a7, // a0
	0x00a8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00ad, 0x017d, 0x00af, // a8
	0x00b0, 0x0105, 0x02db, 0x0157, 0x00b4, 0x0129, 0x013c, 0x02c7, // b0
	0x00b8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014a, 0x017e, 0x014b, // b8
	0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e, // c0
	0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x012a, // c8
	0x0110, 0x0145, 0x014c, 0x0136, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
	0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x0168, 0x016a, 0x00df, // d8
	0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f, // e0
	0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x012b, // e8
	0x0111, 0x0146, 0x014d, 0x0137, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
	0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x0169, 0x016b, 0x02d9, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-5.TXT
static const uint32_t ISO_8859_5[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, // a0
	0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x00ad, 0x040e, 0x040f, // a8
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, // b0
	0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f, // b8
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, // c0
	0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f, // c8
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, // d0
	0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f, // d8
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, // e0
	0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f, // e8
	0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, // f0
	0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x00a7, 0x045e, 0x045f, // f8
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
	case ESPEAKNG_ENCODING_ISO_8859_3:
		decoder->get = text_decoder_getc_codepage;
		decoder->codepage = ISO_8859_3;
		break;
	case ESPEAKNG_ENCODING_ISO_8859_4:
		decoder->get = text_decoder_getc_codepage;
		decoder->codepage = ISO_8859_4;
		break;
	case ESPEAKNG_ENCODING_ISO_8859_5:
		decoder->get = text_decoder_getc_codepage;
		decoder->codepage = ISO_8859_5;
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
